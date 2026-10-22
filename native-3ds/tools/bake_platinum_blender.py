# Run only through Blender; bake supplied source models without reauthoring.
import bpy,sys,json,math
from mathutils import Vector
rows=json.load(open(sys.argv[sys.argv.index('--')+1]))
for row in rows:
 bpy.ops.wm.read_factory_settings(use_empty=True)
 for item in row['items']:
  before=set(bpy.context.scene.objects);bpy.ops.import_scene.gltf(filepath=item['model'])
  roots=[o for o in set(bpy.context.scene.objects)-before if o.parent is None]
  # glTF Y-up -> Blender Z-up: original (x,y,z) becomes (x,-z,y).
  for root in roots:
   root.location+=Vector((item['position'][0],-item['position'][2],item['position'][1]));root.scale=item['scale'];root.rotation_euler.z=-item['rotation'][1]*math.tau/65536
  # The conversion may include a debug skeleton; do not render it.
  for o in set(bpy.context.scene.objects)-before:
   if o.name.startswith('Icosphere'):o.hide_render=True
   if o.type=='MESH':
    for mat in o.data.materials:
     if not mat or not mat.use_nodes:continue
     nodes=mat.node_tree.nodes;links=mat.node_tree.links;image=next((n for n in nodes if n.type=='TEX_IMAGE'),None)
     if image:
      image.interpolation='Closest';em=nodes.new('ShaderNodeEmission');output=next(n for n in nodes if n.type=='OUTPUT_MATERIAL');links.new(image.outputs['Color'],em.inputs['Color']);links.new(em.outputs[0],output.inputs['Surface'])
 camdata=bpy.data.cameras.new('OriginalGeometryProjection');cam=bpy.data.objects.new('Camera',camdata);bpy.context.collection.objects.link(cam)
 cam.location=(0,0,2000);cam.rotation_euler=(0,0,0);camdata.type='ORTHO';camdata.ortho_scale=512;camdata.clip_end=4000
 scene=bpy.context.scene;scene.camera=cam;scene.render.engine='CYCLES';scene.cycles.samples=1;scene.cycles.use_denoising=False;scene.render.resolution_x=512;scene.render.resolution_y=512;scene.render.resolution_percentage=100
 scene.render.image_settings.file_format='PNG';scene.render.film_transparent=False;scene.world=bpy.data.worlds.new('World');scene.world.color=(0,0,0)
 scene.view_settings.view_transform='Standard';scene.render.filepath=row['output'];bpy.ops.render.render(write_still=True)
 print('BAKED',row['output'],flush=True)
