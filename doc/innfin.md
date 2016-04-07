---
layout: page
title: Innere Finsternis
description: A Roguelike Game
ads: false
permalink: /innfin/
published: false
---

*Innfin - Innere Finsternis* is a horror-themed fantasy role-playing
game set in the underworld of dungeons carved into stone and earth. It
is a roguelike game in the tradition of the classics of the genre,
like ADOM, Nethack, DCSS and others. As you play the game, you
discover various creatures in the halls and dungeons hidden from the
world above. Your goal is to choose the deity to worship and attempt
to do his or her bidding. You can abandon your god and convert to
other religion later in the game and have the objective of the game
changed, as well.

{:toc}

# The World

The game is set in the world deep in the past in which magic
reigned. Not even rudimentary civilization was yet conceived by most
races. Writing wasn't invented and songs were known but to the
cleverest of race.

## Portals and Dungeons of Fate

The world consists of dungeons of various levels and portals that are
accessible from dungeons. There are many rules by which portals appear
and where they take you to. This depends on various factors, such as
character race, playing style, religion but it does *not* depend on
character level or skill set, so it isn't always easy to tell whether
a particular portal will take you somewhere you are good to survive or
not. Apart from arbitrary portals, there are some that have more
important role in character development and game progression, such
portals are very few and are easy to identify. You should know when to
enter them and in what order.

Dungeons of Fate are dungeons that do not follow the pattern of
increasing difficulty typical for arbitrary dungeons. You know that
you are in a dungeon of fate when you have made some god angry or
extremely pleased. You are taken to one of the Dungeons of Fate when
you have done something of importance on the large scale.

# Character race

## Dark Elf

Dark elves are the most mysterious of the lot. Seeing their innate
stats, it is difficult to foresee where they would thrive and where
they would perish. They can be outstanding ranged combatants with
their mastery of archery and singing spells. They are, however, hated
by many creatures and feared by yet more.

## Dwarf

Dwarves are the most typical melee fighters with high health points,
good defence and high damage. This doesn't mean that they have much
lower strength in other stats, but this advantage in melee fighting
drives them (or the player manipulating them&#x2026;) into further
developing this skill and therefore neglecting some other skills. This
makes them stand out in early to mid game but facing significant
unforeseen challenges in late game if overly specialized.

## Human

In many games humans are typically the most *generic* species with
stats closest to averages. In this game, however, human species are
treated specially as well, but in a slightly different sense. Humans
have the highest learning capacity and they are more flexible in what
talents they develop. This makes them *generic* in a similar sense to
other roguelikes. The difference lies in that humans are the most
prone to *corruption* and most vulnerable to *mutation*. This can be
both, helpful or hindering based on playing style, situation or
strategy.

## Huttler

Huttlers are distant relatives of humans, possessing similar
intelligence, person and susceptibility toward mutation and corruption
(and other conditions of living) but have vastly different workings in
learning and interactions with non-player characters. The difference
between them and humans becomes more evident as the game progresses.

Some say that Huttlers parallel hurthlings or hobbits in other
media. It hasn't been confirmed, however, and despite the fact that
they indeed live in holes or *huts*, they are more inclined to
barbarism and wilderness than to farming and agriculture. Neither are
they oblivious to the tidings from the larger world.

# Skills

# Combat

## Weapons

## Armory
# Religion

## Gods

## Magic

# Monsters

Unlike many other roguelikes where most monsters are immediately your
enemies, in this game your playing style determines what other
characters see you as. There are, however, some monsters that are
inherently aggressive toward your race or to anything at all. Such
monsters are often *cursed*. 

## Orcs

## Trolls

## Elves

## Specials

## Wizards

Wizards possess similar abilities as the player in that that they can
use magic, use weapons and have advanced tactics in wars.

## Animals

### Intelligent animals

### Cursed animals

Cursed animals serve some fell purpose in the game. Typically they are
cursed to serve some other monsters or gods. To a player, this means
that such an animal can pose significantly more threat than
others. Not all animals can be identified as cursed easily.

# Conditions affecting living

## Food

## Health

## Corruption

## Confusion

## Blindness

Any character can be affected by blindness. None are exempt. When a
player character becomes blind, the field of vision becomes a gray
cloud of nothingness in which no monster, door or item can be
seen. Blindness can be temporary that heals itself in several turns or
semi-permanent, when it lasts until you sing a Bard's Song, drink
*potion of seeing*, invoke a spell or drink a potion that cures
blindness or ask your god.

## Agony

Agony affects characters of different race differently. Your skills,
level, items are all irrelevant. What your character does in agony
depends solely on its race. Agony cannot be cured immediately by
drinking a potion, singing a song or invoking a spell. It doesn't cure
at all sometimes but its effects become less strong when other states
increase or decrease, such as corruption or mutation. Typically, there
is nothing that effectively reduced agony. But you can definitely grow
your agony by any of the following (depending on your race) - resting,
mass killing other monsters, invoking actions that harm you,
excessively drinking potions (not waiting a single turn between
drinking potions).

## Deafness

When you are deaf, you stop sensing danger when attempting to open
doors or initiating combat with a monster that has its allies
nearby. You rarely become deaf but it can be very dangerous if it
happens in critical situations.

## Mutation

# Pickable Items

# Developer's Guide

## Source code structure

### *actor.h*

This header file contains all the functions related to the actors,
including NPC and items (potions, scrolls, books, etc). All of this is
implemented as a single actor structure (struct actor) which contains
pointers to various helper structures, such as \`pickable\` (an item
that can be picked and used, \`destructible\` (an actor that can be
harmed when attacked, \`attacker\`, (an actor that can
attack. Intelligently or with random motives). There is also an
important \`ai\` helper structure, which, if the actor possesses it,
gives the wearer the ability to have intelligence, perhaps even
increasing one, such as capacity for learning. 

# License
