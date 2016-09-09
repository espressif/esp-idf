#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cJSON_Utils.h"

static int cJSONUtils_strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*(const unsigned char *)s1) == tolower(*(const unsigned char *)s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

/* JSON Pointer implementation: */
static int cJSONUtils_Pstrcasecmp(const char *a,const char *e)
{
	if (!a || !e) return (a==e)?0:1;
	for (;*a && *e && *e!='/';a++,e++) {
		if (*e=='~') {if (!(e[1]=='0' && *a=='~') && !(e[1]=='1' && *a=='/')) return 1;  else e++;}
		else if (tolower(*(const unsigned char *)a)!=tolower(*(const unsigned char *)e)) return 1;
	}
	if ((*e!=0 && *e!='/') != (*a!=0)) return 1;
	return 0;
}

static int cJSONUtils_PointerEncodedstrlen(const char *s)	{int l=0;for (;*s;s++,l++) if (*s=='~' || *s=='/') l++;return l;}

static void cJSONUtils_PointerEncodedstrcpy(char *d,const char *s)
{
	for (;*s;s++)
	{
		if (*s=='/') {*d++='~';*d++='1';}
		else if (*s=='~') {*d++='~';*d++='0';}
		else *d++=*s;
	}
	*d=0;
}

char *cJSONUtils_FindPointerFromObjectTo(cJSON *object,cJSON *target)
{
	int type=object->type,c=0;cJSON *obj=0;

	if (object==target) return strdup("");

	for (obj=object->child;obj;obj=obj->next,c++)
	{
		char *found=cJSONUtils_FindPointerFromObjectTo(obj,target);
		if (found)
		{
			if (type==cJSON_Array)
			{
				char *ret=(char*)malloc(strlen(found)+23);
				sprintf(ret,"/%d%s",c,found);
				free(found);
				return ret;
			}
			else if (type==cJSON_Object)
			{
				char *ret=(char*)malloc(strlen(found)+cJSONUtils_PointerEncodedstrlen(obj->string)+2);
				*ret='/';cJSONUtils_PointerEncodedstrcpy(ret+1,obj->string);
				strcat(ret,found);
				free(found);
				return ret;
			}
			free(found);
			return 0;
		}
	}
	return 0;
}

cJSON *cJSONUtils_GetPointer(cJSON *object,const char *pointer)
{
	while (*pointer++=='/' && object)
	{
		if (object->type==cJSON_Array)
		{
			int which=0; while (*pointer>='0' && *pointer<='9') which=(10*which) + *pointer++ - '0';
			if (*pointer && *pointer!='/') return 0;
			object=cJSON_GetArrayItem(object,which);
		}
		else if (object->type==cJSON_Object)
		{
			object=object->child;	while (object && cJSONUtils_Pstrcasecmp(object->string,pointer)) object=object->next;	/* GetObjectItem. */
			while (*pointer && *pointer!='/') pointer++;
		}
		else return 0;
	}
	return object;
}

/* JSON Patch implementation. */
static void cJSONUtils_InplaceDecodePointerString(char *string)
{
	char *s2=string;
	for (;*string;s2++,string++) *s2=(*string!='~')?(*string):((*(++string)=='0')?'~':'/');
	*s2=0;
}

static cJSON *cJSONUtils_PatchDetach(cJSON *object,const char *path)
{
	char *parentptr=0,*childptr=0;cJSON *parent=0,*ret=0;

	parentptr=strdup(path);	childptr=strrchr(parentptr,'/');	if (childptr) *childptr++=0;
	parent=cJSONUtils_GetPointer(object,parentptr);
	cJSONUtils_InplaceDecodePointerString(childptr);

	if (!parent) ret=0;	/* Couldn't find object to remove child from. */
	else if (parent->type==cJSON_Array)		ret=cJSON_DetachItemFromArray(parent,atoi(childptr));
	else if (parent->type==cJSON_Object)	ret=cJSON_DetachItemFromObject(parent,childptr);
	free(parentptr);
	return ret;
}

static int cJSONUtils_Compare(cJSON *a,cJSON *b)
{
	if (a->type!=b->type)	return -1;	/* mismatched type. */
	switch (a->type)
	{
	case cJSON_Number:	return (a->valueint!=b->valueint || a->valuedouble!=b->valuedouble)?-2:0;	/* numeric mismatch. */
	case cJSON_String:	return (strcmp(a->valuestring,b->valuestring)!=0)?-3:0;						/* string mismatch. */
	case cJSON_Array:	for (a=a->child,b=b->child;a && b;a=a->next,b=b->next)	{int err=cJSONUtils_Compare(a,b);if (err) return err;}
						return (a || b)?-4:0;	/* array size mismatch. */
	case cJSON_Object:
						cJSONUtils_SortObject(a);
						cJSONUtils_SortObject(b);
						a=a->child,b=b->child;
						while (a && b)
						{
							int err;
							if (cJSONUtils_strcasecmp(a->string,b->string))	return -6;	/* missing member */
							err=cJSONUtils_Compare(a,b);if (err) return err;
							a=a->next,b=b->next;
						}
						return (a || b)?-5:0;	/* object length mismatch */

	default:			break;
	}
	return 0;
}

static int cJSONUtils_ApplyPatch(cJSON *object,cJSON *patch)
{
	cJSON *op=0,*path=0,*value=0,*parent=0;int opcode=0;char *parentptr=0,*childptr=0;

	op=cJSON_GetObjectItem(patch,"op");
	path=cJSON_GetObjectItem(patch,"path");
	if (!op || !path) return 2;	/* malformed patch. */

	if		(!strcmp(op->valuestring,"add"))	opcode=0;
	else if (!strcmp(op->valuestring,"remove")) opcode=1;
	else if (!strcmp(op->valuestring,"replace"))opcode=2;
	else if (!strcmp(op->valuestring,"move"))	opcode=3;
	else if (!strcmp(op->valuestring,"copy"))	opcode=4;
	else if (!strcmp(op->valuestring,"test"))	return cJSONUtils_Compare(cJSONUtils_GetPointer(object,path->valuestring),cJSON_GetObjectItem(patch,"value"));
	else return 3; /* unknown opcode. */

	if (opcode==1 || opcode==2)	/* Remove/Replace */
	{
		cJSON_Delete(cJSONUtils_PatchDetach(object,path->valuestring));	/* Get rid of old. */
		if (opcode==1) return 0;	/* For Remove, this is job done. */
	}

	if (opcode==3 || opcode==4)	/* Copy/Move uses "from". */
	{
		cJSON *from=cJSON_GetObjectItem(patch,"from");	if (!from) return 4; /* missing "from" for copy/move. */

		if (opcode==3) value=cJSONUtils_PatchDetach(object,from->valuestring);
		if (opcode==4) value=cJSONUtils_GetPointer(object,from->valuestring);
		if (!value) return 5; /* missing "from" for copy/move. */
		if (opcode==4) value=cJSON_Duplicate(value,1);
		if (!value) return 6; /* out of memory for copy/move. */
	}
	else	/* Add/Replace uses "value". */
	{
		value=cJSON_GetObjectItem(patch,"value");
		if (!value) return 7; /* missing "value" for add/replace. */
		value=cJSON_Duplicate(value,1);
		if (!value) return 8; /* out of memory for add/replace. */
	}
		
	/* Now, just add "value" to "path". */

	parentptr=strdup(path->valuestring);	childptr=strrchr(parentptr,'/');	if (childptr) *childptr++=0;
	parent=cJSONUtils_GetPointer(object,parentptr);
	cJSONUtils_InplaceDecodePointerString(childptr);

	/* add, remove, replace, move, copy, test. */
	if (!parent) {free(parentptr); cJSON_Delete(value); return 9;}	/* Couldn't find object to add to. */
	else if (parent->type==cJSON_Array)
	{
		if (!strcmp(childptr,"-"))	cJSON_AddItemToArray(parent,value);
		else						cJSON_InsertItemInArray(parent,atoi(childptr),value);
	}
	else if (parent->type==cJSON_Object)
	{
		cJSON_DeleteItemFromObject(parent,childptr);
		cJSON_AddItemToObject(parent,childptr,value);
	}
	else
	{
		cJSON_Delete(value);
	}
	free(parentptr);
	return 0;
}


int cJSONUtils_ApplyPatches(cJSON *object,cJSON *patches)
{
	int err;
	if (patches->type!=cJSON_Array) return 1;	/* malformed patches. */
	if (patches) patches=patches->child;
	while (patches)
	{
		if ((err=cJSONUtils_ApplyPatch(object,patches))) return err;
		patches=patches->next;
	}
	return 0;
}

static void cJSONUtils_GeneratePatch(cJSON *patches,const char *op,const char *path,const char *suffix,cJSON *val)
{
	cJSON *patch=cJSON_CreateObject();
	cJSON_AddItemToObject(patch,"op",cJSON_CreateString(op));
	if (suffix)
	{
		char *newpath=(char*)malloc(strlen(path)+cJSONUtils_PointerEncodedstrlen(suffix)+2);
		cJSONUtils_PointerEncodedstrcpy(newpath+sprintf(newpath,"%s/",path),suffix);
		cJSON_AddItemToObject(patch,"path",cJSON_CreateString(newpath));
		free(newpath);
	}
	else	cJSON_AddItemToObject(patch,"path",cJSON_CreateString(path));
	if (val) cJSON_AddItemToObject(patch,"value",cJSON_Duplicate(val,1));
	cJSON_AddItemToArray(patches,patch);
}

void cJSONUtils_AddPatchToArray(cJSON *array,const char *op,const char *path,cJSON *val)	{cJSONUtils_GeneratePatch(array,op,path,0,val);}

static void cJSONUtils_CompareToPatch(cJSON *patches,const char *path,cJSON *from,cJSON *to)
{
	if (from->type!=to->type)	{cJSONUtils_GeneratePatch(patches,"replace",path,0,to);	return;	}
	
	switch (from->type)
	{
	case cJSON_Number:	
		if (from->valueint!=to->valueint || from->valuedouble!=to->valuedouble)
			cJSONUtils_GeneratePatch(patches,"replace",path,0,to);
		return;
						
	case cJSON_String:	
		if (strcmp(from->valuestring,to->valuestring)!=0)
			cJSONUtils_GeneratePatch(patches,"replace",path,0,to);
		return;

	case cJSON_Array:
	{
		int c;char *newpath=(char*)malloc(strlen(path)+23);	/* Allow space for 64bit int. */
		for (c=0,from=from->child,to=to->child;from && to;from=from->next,to=to->next,c++){
										sprintf(newpath,"%s/%d",path,c);	cJSONUtils_CompareToPatch(patches,newpath,from,to);
		}
		for (;from;from=from->next,c++)	{sprintf(newpath,"%d",c);	cJSONUtils_GeneratePatch(patches,"remove",path,newpath,0);	}
		for (;to;to=to->next,c++)		cJSONUtils_GeneratePatch(patches,"add",path,"-",to);
		free(newpath);
		return;
	}

	case cJSON_Object:
	{
		cJSON *a,*b;
		cJSONUtils_SortObject(from);
		cJSONUtils_SortObject(to);
		
		a=from->child,b=to->child;
		while (a || b)
		{
			int diff=(!a)?1:(!b)?-1:cJSONUtils_strcasecmp(a->string,b->string);
			if (!diff)
			{
				char *newpath=(char*)malloc(strlen(path)+cJSONUtils_PointerEncodedstrlen(a->string)+2);
				cJSONUtils_PointerEncodedstrcpy(newpath+sprintf(newpath,"%s/",path),a->string);
				cJSONUtils_CompareToPatch(patches,newpath,a,b);
				free(newpath);
				a=a->next;
				b=b->next;
			}
			else if (diff<0)	{cJSONUtils_GeneratePatch(patches,"remove",path,a->string,0);	a=a->next;}
			else				{cJSONUtils_GeneratePatch(patches,"add",path,b->string,b);		b=b->next;}
		}
		return;
	}

	default:			break;
	}
}


cJSON* cJSONUtils_GeneratePatches(cJSON *from,cJSON *to)
{
	cJSON *patches=cJSON_CreateArray();	
	cJSONUtils_CompareToPatch(patches,"",from,to);
	return patches;
}


static cJSON *cJSONUtils_SortList(cJSON *list)
{
	cJSON *first=list,*second=list,*ptr=list;

	if (!list || !list->next) return list;	/* One entry is sorted already. */
	
	while (ptr && ptr->next && cJSONUtils_strcasecmp(ptr->string,ptr->next->string)<0) ptr=ptr->next;	/* Test for list sorted. */
	if (!ptr || !ptr->next) return list;	/* Leave sorted lists unmodified. */
	ptr=list;

	while (ptr) {second=second->next;ptr=ptr->next;if (ptr) ptr=ptr->next;}	/* Walk two pointers to find the middle. */
	if (second && second->prev) second->prev->next=0;	/* Split the lists */

	first=cJSONUtils_SortList(first);	/* Recursively sort the sub-lists. */
	second=cJSONUtils_SortList(second);
	list=ptr=0;

	while (first && second)	/* Merge the sub-lists */
	{		
		if (cJSONUtils_strcasecmp(first->string,second->string)<0)
		{
			if (!list) list=ptr=first;
			else	{ptr->next=first;first->prev=ptr;ptr=first;}
			first=first->next;
		}
		else 
		{
			if (!list) list=ptr=second;
			else	{ptr->next=second;second->prev=ptr;ptr=second;}
			second=second->next;
		}
	}
	if (first)	{	if (!list) return first;	ptr->next=first;	first->prev=ptr;	}	/* Append any tails. */
	if (second)	{	if (!list) return second;	ptr->next=second;	second->prev=ptr;	}

	return list;
}

void cJSONUtils_SortObject(cJSON *object)	{object->child=cJSONUtils_SortList(object->child);}

cJSON* cJSONUtils_MergePatch(cJSON *target, cJSON *patch)
{
	if (!patch || patch->type != cJSON_Object) {cJSON_Delete(target);return cJSON_Duplicate(patch,1);}
	if (!target || target->type != cJSON_Object) {cJSON_Delete(target);target=cJSON_CreateObject();}

	patch=patch->child;
	while (patch)
	{
		if (patch->type == cJSON_NULL) cJSON_DeleteItemFromObject(target,patch->string);
		else
		{
			cJSON *replaceme=cJSON_DetachItemFromObject(target,patch->string);
			cJSON_AddItemToObject(target,patch->string,cJSONUtils_MergePatch(replaceme,patch));
		}
		patch=patch->next;
	}
	return target;
}

cJSON *cJSONUtils_GenerateMergePatch(cJSON *from,cJSON *to)
{
	cJSON *patch=0;
	if (!to) return cJSON_CreateNull();
	if (to->type!=cJSON_Object || !from || from->type!=cJSON_Object) return cJSON_Duplicate(to,1);
	cJSONUtils_SortObject(from);
	cJSONUtils_SortObject(to);
	from=from->child;to=to->child;
	patch=cJSON_CreateObject();
	while (from || to)
	{
		int compare=from?(to?strcmp(from->string,to->string):-1):1;
		if (compare<0)
		{
			cJSON_AddItemToObject(patch,from->string,cJSON_CreateNull());
			from=from->next;
		}
		else if (compare>0)
		{
			cJSON_AddItemToObject(patch,to->string,cJSON_Duplicate(to,1));
			to=to->next;
		}
		else
		{
			if (cJSONUtils_Compare(from,to)) cJSON_AddItemToObject(patch,to->string,cJSONUtils_GenerateMergePatch(from,to));
			from=from->next;to=to->next;
		}
	}
	if (!patch->child) {cJSON_Delete(patch);return 0;}
	return patch;
}