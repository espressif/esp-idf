#include "cJSON.h"

/* Implement RFC6901 (https://tools.ietf.org/html/rfc6901) JSON Pointer spec.	*/
cJSON *cJSONUtils_GetPointer(cJSON *object,const char *pointer);

/* Implement RFC6902 (https://tools.ietf.org/html/rfc6902) JSON Patch spec.		*/
cJSON* cJSONUtils_GeneratePatches(cJSON *from,cJSON *to);
void cJSONUtils_AddPatchToArray(cJSON *array,const char *op,const char *path,cJSON *val);	/* Utility for generating patch array entries.	*/
int cJSONUtils_ApplyPatches(cJSON *object,cJSON *patches);	/* Returns 0 for success. */

/*
// Note that ApplyPatches is NOT atomic on failure. To implement an atomic ApplyPatches, use:
//int cJSONUtils_AtomicApplyPatches(cJSON **object, cJSON *patches)
//{
//	cJSON *modme=cJSON_Duplicate(*object,1);
//	int error=cJSONUtils_ApplyPatches(modme,patches);
//	if (!error)	{cJSON_Delete(*object);*object=modme;}
//	else		cJSON_Delete(modme);
//	return error;
//}
// Code not added to library since this strategy is a LOT slower.
*/

/* Implement RFC7386 (https://tools.ietf.org/html/rfc7396) JSON Merge Patch spec. */
cJSON* cJSONUtils_MergePatch(cJSON *target, cJSON *patch); /* target will be modified by patch. return value is new ptr for target. */
cJSON *cJSONUtils_GenerateMergePatch(cJSON *from,cJSON *to); /* generates a patch to move from -> to */

char *cJSONUtils_FindPointerFromObjectTo(cJSON *object,cJSON *target);	/* Given a root object and a target object, construct a pointer from one to the other.	*/

void cJSONUtils_SortObject(cJSON *object);	/* Sorts the members of the object into alphabetical order.	*/
