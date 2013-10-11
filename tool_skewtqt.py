toolname = 'skewtqt'

thisDir = Dir('.').abspath
upDir   = Dir('./../').abspath

# Define tool
def skewtqt(env):
    env.Append(LIBS = toolname)
    env.AppendUnique(CPPPATH = upDir)
    env.AppendUnique(CPPPATH = thisDir)
    env.AppendUnique(LIBPATH = thisDir)
    env.Require(['qt4', 'prefixoptions', 'skewt'])
    env.EnableQt4Modules(['QtCore','QtGui'])
    
Export(toolname)

# Build the library
env = Environment(tools = ['default', 'doxygen', toolname])

sources = Split("""
   SkewTAdapterQt.cc
""")

headers = Split("""
   SkewTAdapterQt.h
""")

lib = env.Library(toolname, sources)
env.Default(lib)

# Create doxygen
doxref = env.Apidocs(sources + headers, DOXYFILE_DICT={'PROJECT_NAME':toolname, 'PROJECT_NUMBER':'1.0'})
