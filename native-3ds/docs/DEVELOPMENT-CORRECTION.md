# Presentation correction and continuing integration

The user rejected the previous native preview's custom field dashboard and
story presentation. The later opening/Battle Lab development package was
delivered separately; it is not the completed adventure. The approved older
releases remain untouched.

Required result: play original Platinum's adventure as the chosen custom
trainer. Original Platinum must own the maps, story, dialogue presentation,
field screens, normal menus and progression. Custom UI belongs at the creator
and Arena battles, with minimal integration entries where needed.

The previous DS source already demonstrates the correct integration boundaries:
`src/applications/rowan_intro/rowan_intro_app.c` embeds the creator in the existing
Rowan introduction; `src/arena/avatar.c` loads/saves the avatar and applies its
appearance to the original player texture. Original field/task/menu systems
continue running. That is the reference for native integration, not a collection
of simplified story objectives or custom dashboards.

Native work so far: original field/story data intake and opening VM, plus combat
improvements. Those do not amount to a port of the entire original game engine.
The complete native Platinum story and original menu services are unfinished.
Removing visible panels alone does not solve that engine gap.

Corrective source work removes the Arena field header, debug map/tile dashboard
and persistent help panels; preserves original text case/Unicode and original
two-line message pagination; imports the actual original proportional font,
message box tiles and original pre-Poketch lower-screen tilemap. Later menu and
story services still need faithful porting. Do not label or distribute this
work as the completed requested game.
