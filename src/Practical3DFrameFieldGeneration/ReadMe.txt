This is a very basic implementation of the frame field initialization, as described in "Practical 3D Frame Field Generation".

---------------Files---------------------------
FF.h             defines the API
FF.cpp           is the code of our algorithm 
OpenNL_psm.h     is an API to solve Least squares problems 
OpenNL_psm.cpp   is the solver's code
main.cpp         is an example that loads a tet mesh and outputs a frame field
*.tet            are example files

---------------Compilation---------------------
To produce the example, just compile *.cpp
To add to a project, just add "FF.cpp" and "OpenNL_psm.cpp" to the project and include "FF.h"

---------------Example usage-------------------
a.out <inputfilename>.tet
will output 
-> out.obj      for viualization: it is a quad surface with a cube oriented 
                by the frame field, for each vertex of the input mesh
-> out.txt      ascii file where each line corresponds to a vertex of the
                original mesh, and shows the rotation matrice. 


---------------Differences with the article----
The objective is to have an implementaton that is easy to read. To acheive this
objective, we did not exactly reproduce our producation code nor hat is described in the article. Here are the main differences:

* we did not include the non-linear part because it adds dependency that makes the project more difficult to compile/use. We hope to be able to distribute it as part of our (free) software.

* locked frame: we do not lock frames for vertices located on hard edges. It requires to have a mesh data stucture with adjacency information and an hard edge detector.

* vertex ordering: inner vertices and boundary vertices are not sorted here. We instead add 2 extra variables (called "C" in the article) for all vertices --- it just produces extra (unused) variables.

* we do not optimize the vertices order with hibert sort.

* we do not use parallelism to convert SH to rotations.

* in the example, we compute vertex normal as the normal of one (random) of its adjacent boundary faces.

