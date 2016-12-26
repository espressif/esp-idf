/*   
Copyright 2006 - 2011 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 * Implements additional string functionality.
 */

#ifndef MY_STRING_H

/*
 *	EndsWith()
 *		str					: the string to analyze
 *		endsWith			: the token to find at the end of str
 *		ignoreCase			: nonzero indicates case insensitive search
 *
 *	If "str" ends with "endsWith", then we return nonzero.
 */
int EndsWith(const char* str, const char* endsWith, int ignoreCase);

/*
 *	IndexOf()
 *		str					: the string to analyze
 *		findThis			: the token to find 
 *		ignoreCase			: nonzero indicates case insensitive search
 *
 *	Returns the first index where findThis can be found in str.
 *	Returns -1 if not found.
 */
int IndexOf(const char* str, const char* findThis);

/*
 *	IndexOf()
 *		str					: the string to analyze
 *		findThis			: the token to find 
 *		ignoreCase			: nonzero indicates case insensitive search
 *
 *	Returns the last index where findThis can be found in str.
 *	Returns -1 if not found.
 */
int LastIndexOf(const char* str, const char* findThis);

/*
 *	IndexOf()
 *		str					: the string to analyze
 *		startsWith			: the token to find 
 *		ignoreCase			: nonzero indicates case insensitive search
 *
 *	Returns nonzero value if the string starts with 'startsWith'.
 */
int StartsWith(const char* str, const char* startsWith, int ignoreCase);

#endif 
