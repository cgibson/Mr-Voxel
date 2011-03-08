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

objects = []

objects = objects + SConscript('src/SConscript') #, build_dir=buildDir)
objects = objects + SConscript('src/scene/SConscript')
objects = objects + SConscript('src/util/SConscript')
objects = objects + SConscript('src/json/SConscript')
objects = objects + env.SConscript('src/mrvoxel/SConscript')
objects = objects + env.SConscript('src/integrator/SConscript')
objects = objects + env.SConscript('src/ocean/SConscript')

if conf.CheckDeclaration("__i386__"):
	objects = objects + ['lib/x86/libjson_linux-gcc-4.4.1_libmt.a'];
else:
	objects = objects + ['lib/x86-64/libjson_linux-gcc-4.4.1_libmt.a'];

env.Program('raytracer', objects, LIBS=['boost_thread-mt'], LIBPATH='.')
