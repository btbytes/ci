#ifndef _EXTERN_G_H
#define _EXTERN_G_H

#define NAME_MAX 80
extern char resultFileName[NAME_MAX], dataFileName[NAME_MAX];
extern int fuzzyFlag; //1: average  0:minimum
extern int defuzzyFlag; //0: maximum, 1: centroid without overlap, 2: with overlap
extern int ruleEffectFlag; //1:maximum 0:add output values from each rule

#endif
