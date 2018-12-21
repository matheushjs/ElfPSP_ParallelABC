#ifndef _CUDA_CONTACT_COUNT__
#define _CUDA_CONTACT_COUNT__

/* XXX: This file mirrors CUDA_collision_count.h Please check it */

#ifndef ELF_FLOAT_TYPES
#define ELF_FLOAT_TYPES
struct CollisionCountPromise {  // Vectors of # of collisions
	int *d_toReduce;
	int *d_reduced;
};

typedef struct {
	float x;
	float y;
	float z;
} ElfFloat3d;

typedef struct {
	int x;
	int y;
	int z;
} ElfInt3d;
#endif // TYPES

struct CollisionCountPromise
	count_contacts_launch(ElfFloat3d *vector, int size);

int count_contacts_fetch(struct CollisionCountPromise promise);

#endif
