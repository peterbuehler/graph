A simple C-library that draws lineplots with direct2d.
Written for windows with Visual Studio 2022.

Files needed:
graph.h  	// public header file
graph.lib 	// static library


Functionality:
- each graph is painted in own window
- graph may have multiple datarows
- linear and logarithmic axes supported
- axes of graph are scaled automatically
- timer to update content periodically
- uses opaque datatypes to hide internas


Code-Style:
- Code is written in C, using C++ syntax.
- C++ syntax is needed for interop with direct2d.