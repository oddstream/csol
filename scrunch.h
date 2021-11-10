/* scrunch.h */

#ifndef SCRUNCH_H
#define SCRUNCH_H

void BaizeCalculateScrunchLimits(struct Baize *const baize, const int windowWidth, const int windowHeight);

_Bool ScrunchOverflow(struct Pile *const pile, Vector2 newpos);
void ScrunchPile(struct Pile *const pile);
void ScrunchPiles(struct Baize *const baize);

#endif
