import eol_scons

libraryName = 'skewtqt'

tools = ['doxygen','qt4','prefixoptions',]
env = Environment(tools = ['default'] + tools)

thisDir = Dir('.').abspath

qt4Modules = Split('QtCore QtGui')

# Define tool
def skewtqt(env):
	env.AppendUnique(LIBS = libraryName)
	env.AppendUnique(CPPPATH = thisDir)
	env.AppendUnique(LIBPATH = thisDir)
	env.AppendDoxref(doxref[0])
	env.EnableQt4Modules(qt4Modules)
	env.Require(tools)
	
Export(libraryName)

# Build library

sources = Split("""
   SkewTAdapterQt.cc
""")

headers = Split("""
   SkewTAdapterQt.h
""")

lib = env.Library(libraryName, sources)
env.Default(lib)

# Create doxygen
doxref = env.Apidocs(sources + headers, DOXYFILE_DICT={'PROJECT_NAME':libraryName, 'PROJECT_NUMBER':'1.0'})

