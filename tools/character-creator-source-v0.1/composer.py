#!/usr/bin/env python3
"""Editable trainer-sheet compositor; this is a source-art tool, not DS code."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import sys

from PIL import Image


LAYERS = ("bottoms", "tops", "hair", "headgear", "accessories")
DIRECTIONS = ("down", "left", "right", "up")
CELL = (32, 48)
CANVAS = (128, 192)
BASE = Path(__file__).resolve().parent


def read_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def source_path(root: Path, value: str) -> Path:
    """Require a relative path which resolves inside this catalogue directory."""
    if not isinstance(value, str) or "\\" in value:
        raise ValueError("Source paths must use relative forward-slash paths")
    raw = PurePosixPath(value)
    if raw.is_absolute() or ".." in raw.parts:
        raise ValueError("Absolute paths and parent-directory traversal are forbidden")
    path = (root / Path(*raw.parts)).resolve()
    if not path.is_relative_to(root.resolve()):
        raise ValueError("Source path resolves outside the catalogue directory")
    return path


def open_sheet(path: Path) -> Image.Image:
    with Image.open(path) as opened:
        image = opened.convert("RGBA")
    if image.size == (128, 196):
        if image.getchannel("A").crop((0, 192, 128, 196)).getbbox():
            raise ValueError(f"{path.name}: extra four rows contain visible artwork")
        image = image.crop((0, 0, 128, 192))
    if image.size != CANVAS:
        raise ValueError(f"{path}: expected a 128x192 walking sheet, got {image.size}")
    return image


def load_catalog(path: Path):
    catalog = read_json(path)
    if catalog.get("schema_version") != 1:
        raise ValueError("Unsupported catalogue schema")
    if catalog.get("canvas") != list(CANVAS) or catalog.get("cell") != list(CELL):
        raise ValueError("This tool requires the source 32x48 walking-cell layout")
    if catalog.get("directions") != list(DIRECTIONS):
        raise ValueError("Direction order must be down, left, right, up")
    order = catalog.get("layer_order", [])
    if len(order) != 6 or set(order) != {"body", *LAYERS} or order[0] != "body":
        raise ValueError("Layer order must begin with body and contain every layer once")
    return catalog


def selection_for(catalog, profile_path=None, overrides=None):
    selection = dict(catalog["default_selection"])
    if profile_path:
        profile = read_json(Path(profile_path))
        if profile.get("schema_version") != 1:
            raise ValueError("Unsupported selection profile schema")
        unknown = set(profile.get("selection", {})) - set(selection)
        if unknown:
            raise ValueError(f"Unknown selection keys: {sorted(unknown)}")
        selection.update(profile.get("selection", {}))
    selection.update({k: str(v) for k, v in (overrides or {}).items() if v is not None})
    return {k: str(v) for k, v in selection.items()}


def offset_for(option, body, direction):
    """Offsets add: all + body + direction + body.direction, in source pixels."""
    result = [0, 0]
    offsets = option.get("offsets", {})
    allowed = {"all", "A", "B", *DIRECTIONS,
               *(f"{b}.{d}" for b in ("A", "B") for d in DIRECTIONS)}
    if set(offsets) - allowed:
        raise ValueError(f"Unknown offset keys: {sorted(set(offsets) - allowed)}")
    for key in ("all", body, direction, f"{body}.{direction}"):
        value = offsets.get(key, [0, 0])
        if (not isinstance(value, list) or len(value) != 2
                or any(type(n) is not int for n in value)):
            raise ValueError(f"Offset {key} must be [integer x, integer y]")
        result[0] += value[0]
        result[1] += value[1]
    return tuple(result)


def positioned_layer(image, option, body):
    result = Image.new("RGBA", CANVAS)
    for row, direction in enumerate(DIRECTIONS):
        dx, dy = offset_for(option, body, direction)
        for col in range(4):
            x, y = col * CELL[0], row * CELL[1]
            cell = image.crop((x, y, x + CELL[0], y + CELL[1]))
            bounds = cell.getchannel("A").getbbox()
            if bounds and (bounds[0] + dx < 0 or bounds[1] + dy < 0
                           or bounds[2] + dx > CELL[0] or bounds[3] + dy > CELL[1]):
                raise ValueError(f"Offset clips artwork for {body}.{direction} frame {col}")
            # Paste into the existing fixed grid, never recenter on visible pixels.
            result.alpha_composite(cell, (x + dx, y + dy))
    return result


def compose(catalog_path: Path, catalog, selection):
    root = catalog_path.resolve().parent
    body, skin = selection["body"], selection["skin"]
    if body not in catalog["bodies"] or skin not in catalog["bodies"][body]["skins"]:
        raise ValueError("Unknown body or skin selection")
    record = catalog["bodies"][body]["skins"][skin]
    records = [("body", record, {})]
    for name in catalog["layer_order"][1:]:
        options = catalog["layers"][name]
        option_id = selection[name]
        if option_id not in options:
            raise ValueError(f"Unknown {name} option: {option_id}")
        option = options[option_id]
        if body not in option["files"]:
            raise ValueError(f"{name}/{option_id} has no matching body {body} sheet")
        records.append((name, option["files"][body], option))
    canvas = Image.new("RGBA", CANVAS)
    inputs = []
    for name, record, option in records:
        path = source_path(root, record["path"])
        data = path.read_bytes()
        image = open_sheet(path)
        if name != "body":
            image = positioned_layer(image, option, body)
        canvas = Image.alpha_composite(canvas, image)
        digest = hashlib.sha256(data).hexdigest()
        inputs.append({"layer": name, "path": record["path"], "sha256": digest,
                       "matches_original": digest == record.get("sha256"),
                       "offsets": option.get("offsets", {})})
    return canvas, inputs


def preview_image(sheet, direction, frame, scale):
    if direction not in DIRECTIONS or frame not in range(4) or scale not in range(1, 13):
        raise ValueError("Preview requires a known direction, frame 0..3 and scale 1..12")
    x, y = frame * 32, DIRECTIONS.index(direction) * 48
    return sheet.crop((x, y, x + 32, y + 48)).resize((32 * scale, 48 * scale), Image.Resampling.NEAREST)


def writable_output(path: Path, catalog_path: Path):
    path = path.resolve()
    root = catalog_path.resolve().parent
    if path == catalog_path.resolve() or path.is_relative_to((root / "sources").resolve()):
        raise ValueError("Export must not overwrite the catalogue or source artwork")
    path.parent.mkdir(parents=True, exist_ok=True)
    return path


def export(catalog_path, catalog, selection, output, preview=None,
           direction="down", frame=0, scale=4, profile_output=None):
    output = writable_output(Path(output), catalog_path)
    profile_output = writable_output(Path(profile_output) if profile_output else output.with_suffix(".profile.json"), catalog_path)
    receipt = writable_output(output.with_suffix(".export.json"), catalog_path)
    preview_path = writable_output(Path(preview), catalog_path) if preview else None
    paths = [output, profile_output, receipt] + ([preview_path] if preview_path else [])
    if len(set(paths)) != len(paths):
        raise ValueError("Sheet, profile, receipt and preview must use distinct paths")
    image, inputs = compose(catalog_path, catalog, selection)
    preview_art = preview_image(image, direction, frame, scale) if preview_path else None
    image.save(output, format="PNG")
    profile = {"schema_version": 1, "selection": selection}
    profile_output.write_text(json.dumps(profile, indent=2) + "\n", encoding="utf-8")
    if preview_path:
        preview_art.save(preview_path, format="PNG")
    colours = {pixel[:3] for pixel in image.getdata() if pixel[3]}
    result = {"schema_version": 1, "tool": "source-art-compositor",
              "rom_integration": False, "selection": selection,
              "canvas": list(CANVAS), "cell": list(CELL),
              "directions": list(DIRECTIONS), "layer_order": catalog["layer_order"],
              "visible_rgb_colours": len(colours), "fits_single_15_colour_palette": len(colours) <= 15,
              "partial_alpha_pixels": sum(0 < p[3] < 255 for p in image.getdata()),
              "sources": inputs, "sheet": str(output),
              "sheet_sha256": hashlib.sha256(output.read_bytes()).hexdigest(),
              "profile": str(profile_output), "preview": str(preview_path) if preview_path else None}
    receipt.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    return result


def gui(catalog_path, catalog):
    import tkinter as tk
    from tkinter import ttk, filedialog, messagebox
    from PIL import ImageTk

    app = tk.Tk()
    app.title("Trainer source-art editor — not a ROM preview")
    values = {k: tk.StringVar(value=str(v)) for k, v in catalog["default_selection"].items()}
    values["direction"] = tk.StringVar(value="down")
    values["frame"] = tk.StringVar(value="0")
    preview = ttk.Label(app)
    preview.grid(row=0, column=2, rowspan=10, padx=18)
    note = tk.StringVar(value="")
    ttk.Label(app, textvariable=note, wraplength=440).grid(row=11, column=0, columnspan=3, padx=8, pady=8)
    controls = [("body", list(catalog["bodies"])), ("skin", list(catalog["bodies"]["A"]["skins"]))]
    controls += [(name, list(catalog["layers"][name])) for name in LAYERS]
    controls += [("direction", list(DIRECTIONS)), ("frame", [str(n) for n in range(4)])]

    def selected():
        return {k: values[k].get() for k in catalog["default_selection"]}

    def redraw(*_):
        try:
            image, _ = compose(catalog_path, catalog, selected())
            shown = ImageTk.PhotoImage(preview_image(image, values["direction"].get(), int(values["frame"].get()), 4))
            preview.configure(image=shown)
            preview.image = shown
            count = len({p[:3] for p in image.getdata() if p[3]})
            note.set(f"Source preview: {count} colours. Native DS palette/rig conversion is separate. No animation or ROM is running.")
        except (OSError, ValueError, KeyError) as error:
            note.set(str(error))

    for row, (name, choices) in enumerate(controls):
        ttk.Label(app, text=name.title()).grid(row=row, column=0, sticky="w", padx=8, pady=3)
        box = ttk.Combobox(app, textvariable=values[name], values=choices, state="readonly", width=18)
        box.grid(row=row, column=1, padx=8)
        values[name].trace_add("write", redraw)

    def save():
        path = filedialog.asksaveasfilename(defaultextension=".png", filetypes=[("PNG sheet", "*.png")])
        if not path:
            return
        try:
            export(catalog_path, catalog, selected(), path)
            note.set(f"Saved sheet and editable profile: {path}")
        except (OSError, ValueError, KeyError) as error:
            messagebox.showerror("Cannot export", str(error))

    ttk.Button(app, text="Export sheet + profile", command=save).grid(row=10, column=0, columnspan=2, pady=8)
    redraw()
    app.mainloop()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--catalog", type=Path, default=BASE / "catalog.json")
    commands = parser.add_subparsers(dest="command", required=True)
    listing = commands.add_parser("list", help="List editable option IDs")
    listing.add_argument("--layer", choices=LAYERS)
    commands.add_parser("gui", help="Open a local Tkinter selection menu")
    rendering = commands.add_parser("render", help="Compose original-size PNG sheet")
    rendering.add_argument("--profile", type=Path)
    rendering.add_argument("--body", choices=("A", "B"))
    rendering.add_argument("--skin", choices=("0", "1", "2", "3"))
    for name in LAYERS:
        aliases = {"tops": "--top", "bottoms": "--bottom", "headgear": "--hat", "accessories": "--accessory"}
        flags = [f"--{name}"] + ([aliases[name]] if name in aliases else [])
        rendering.add_argument(*flags, dest=name)
    rendering.add_argument("--output", type=Path, required=True)
    rendering.add_argument("--save-profile", type=Path)
    rendering.add_argument("--preview", type=Path)
    rendering.add_argument("--direction", choices=DIRECTIONS, default="down")
    rendering.add_argument("--frame", type=int, choices=range(4), default=0)
    rendering.add_argument("--scale", type=int, choices=range(1, 13), default=4)
    args = parser.parse_args()
    try:
        catalog_path = args.catalog.resolve()
        catalog = load_catalog(catalog_path)
        if args.command == "list":
            names = [args.layer] if args.layer else list(LAYERS)
            print(json.dumps({"bodies": list(catalog["bodies"]), "skins": ["0", "1", "2", "3"],
                              "layers": {name: {key: value["label"] for key, value in catalog["layers"][name].items()} for name in names}}, indent=2))
        elif args.command == "gui":
            gui(catalog_path, catalog)
        else:
            overrides = {name: getattr(args, name) for name in ("body", "skin", *LAYERS)}
            selection = selection_for(catalog, args.profile, overrides)
            result = export(catalog_path, catalog, selection, args.output, args.preview,
                            args.direction, args.frame, args.scale, args.save_profile)
            print(json.dumps(result, indent=2))
    except (OSError, ValueError, KeyError) as error:
        parser.exit(2, f"Error: {error}\n")


if __name__ == "__main__":
    main()
