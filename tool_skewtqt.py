toolname = 'skewtqt'

thisDir = Dir('.').abspath
upDir   = Dir('./../').abspath

# Define tool
def skewtqt(env):
    env.Append(LIBS = [toolname])
    env.AppendUnique(CPPPATH = upDir)
    env.AppendUnique(CPPPATH = thisDir)
    env.AppendUnique(LIBPATH = thisDir)
    qt_tool = 'qt4'
    if env.get('QT_VERSION', 4) != 4:
        qt_tool = 'qt5'
    env.Require([qt_tool, 'prefixoptions', 'skewt'])
    env.EnableQtModules(['QtCore','QtGui'])
    
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
