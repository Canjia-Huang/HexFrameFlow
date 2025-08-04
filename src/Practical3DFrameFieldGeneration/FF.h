
void computeFF(
	int nbv,		// number of vertices
	int nbt,		// number of tetrahedrons
	double *n1,		// INPUT: n1[3*v+d] is the d^{th} coordinate of the first locked normal of vertex v
	int	*tet,		// INPUT: tet[4*t+] is the i^{th} vertex d of tet t
	double	*rot	// OUTPUT: rot[9*v+3*e+d] is the d^{th} coordinate of the e^{th} vector of vertex v
);
