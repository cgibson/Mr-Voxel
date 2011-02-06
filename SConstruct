import platform

env = Environment(CC = 'g++')
env.Append(CPPPATH=['include'])
conf = Configure(env)

env["CXXFLAGS"] = "-std=c++0x"

env["CXXFLAGS"] = "-O3"


d = env.ParseFlags("-g")
env.MergeFlags(d)

Export('env')

#buildDir = 'build'

env.SConscript('src/SConscript') #, build_dir=buildDir)
env.SConscript('src/scene/SConscript')
env.SConscript('src/util/SConscript')
env.SConscript('src/json/SConscript')
env.SConscript('src/mrvoxel/SConscript')
env.SConscript('src/integrator/SConscript')

objects = Glob('src/*.o') + \
          Glob('src/scene/*.o') + \
          Glob('src/util/*.o') + \
          Glob('src/json/*.o') + \
          Glob('src/mrvoxel/*.o') + \
          Glob('src/integrator/*.o')

if conf.CheckDeclaration("__i386__"):
	objects = objects + ['lib/x86/libjson_linux-gcc-4.4.1_libmt.so'];
else:
	objects = objects + ['lib/x86-64/libjson_linux-gcc-4.4.1_libmt.so'];

env.Program('raytracer', objects, LIBS=['boost_thread-mt'], LIBPATH='.')
