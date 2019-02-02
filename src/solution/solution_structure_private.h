
typedef struct Solution_ {
	MovElem *chain;       /**< Position of such solution */
	double fitness;       /**< Fitness of such solution. Calculated lazily. */
	int idle_iterations;  /**< Number of iterations through which the food didn't improve */
} Solution;

