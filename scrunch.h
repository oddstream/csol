/* scrunch.h */

#ifndef SCRUNCH_H
#define SCRUNCH_H

void BaizeFindBuddyPiles(struct Baize *const baize);
void BaizeCalculateScrunchDims(struct Baize *const baize, const int windowWidth, const int windowHeight);
void ScrunchPile(struct Pile *const pile);
void ScrunchPiles(struct Baize *const baize);
void ScrunchDrawDebug(struct Pile *const pile);

#endif
