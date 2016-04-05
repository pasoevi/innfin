<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. Player's Guide</a>
<ul>
<li><a href="#sec-1-1">1.1. The World</a></li>
<li><a href="#sec-1-2">1.2. Character race</a></li>
<li><a href="#sec-1-3">1.3. Skills</a></li>
<li><a href="#sec-1-4">1.4. Combat</a>
<ul>
<li><a href="#sec-1-4-1">1.4.1. Weapons</a></li>
<li><a href="#sec-1-4-2">1.4.2. Armory</a></li>
</ul>
</li>
<li><a href="#sec-1-5">1.5. Magic</a></li>
<li><a href="#sec-1-6">1.6. Religion</a></li>
<li><a href="#sec-1-7">1.7. Monsters</a>
<ul>
<li><a href="#sec-1-7-1">1.7.1. Orcs</a></li>
<li><a href="#sec-1-7-2">1.7.2. Trolls</a></li>
<li><a href="#sec-1-7-3">1.7.3. Elves</a></li>
<li><a href="#sec-1-7-4">1.7.4. Specials</a></li>
<li><a href="#sec-1-7-5">1.7.5. Wizards</a></li>
<li><a href="#sec-1-7-6">1.7.6. Animals</a></li>
</ul>
</li>
<li><a href="#sec-1-8">1.8. Conditions affecting living</a>
<ul>
<li><a href="#sec-1-8-1">1.8.1. Food</a></li>
<li><a href="#sec-1-8-2">1.8.2. Health</a></li>
<li><a href="#sec-1-8-3">1.8.3. Corruption</a></li>
<li><a href="#sec-1-8-4">1.8.4. Confusion</a></li>
<li><a href="#sec-1-8-5">1.8.5. Blindness</a></li>
<li><a href="#sec-1-8-6">1.8.6. Agony</a></li>
<li><a href="#sec-1-8-7">1.8.7. Deafness</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-2">2. Developer's Guide</a>
<ul>
<li><a href="#sec-2-1">2.1. Source code structure</a>
<ul>
<li><a href="#sec-2-1-1">2.1.1. <i>actor.h</i></a></li>
</ul>
</li>
</ul>
</li>
</ul>
</div>
</div>


# Player's Guide<a id="sec-1" name="sec-1"></a>

## The World<a id="sec-1-1" name="sec-1-1"></a>

The game is set in the world deep in the past in which magic
reigned. Not even rudimentary civilization was yet conceived by most
races. Writing wasn't invented and songs were known but to the
cleverest of race.

## Character race<a id="sec-1-2" name="sec-1-2"></a>

## Skills<a id="sec-1-3" name="sec-1-3"></a>

## Combat<a id="sec-1-4" name="sec-1-4"></a>

### Weapons<a id="sec-1-4-1" name="sec-1-4-1"></a>

### Armory<a id="sec-1-4-2" name="sec-1-4-2"></a>

## Magic<a id="sec-1-5" name="sec-1-5"></a>

## Religion<a id="sec-1-6" name="sec-1-6"></a>

## Monsters<a id="sec-1-7" name="sec-1-7"></a>

Most monsters are 

### Orcs<a id="sec-1-7-1" name="sec-1-7-1"></a>

### Trolls<a id="sec-1-7-2" name="sec-1-7-2"></a>

### Elves<a id="sec-1-7-3" name="sec-1-7-3"></a>

### Specials<a id="sec-1-7-4" name="sec-1-7-4"></a>

### Wizards<a id="sec-1-7-5" name="sec-1-7-5"></a>

Wizards possess similar abilities as the player in that that they can
use magic, use weapons and have advanced tactics in wars.

### Animals<a id="sec-1-7-6" name="sec-1-7-6"></a>

1.  Intelligent animals

2.  Cursed animals

    Cursed animals serve some fell purpose in the game. Typically they are
    cursed to serve some other monsters or gods. To a player, this means
    that such an animal can pose significantly more threat than
    others. Not all animals can be identified as cursed easily. 

## Conditions affecting living<a id="sec-1-8" name="sec-1-8"></a>

### Food<a id="sec-1-8-1" name="sec-1-8-1"></a>

### Health<a id="sec-1-8-2" name="sec-1-8-2"></a>

### Corruption<a id="sec-1-8-3" name="sec-1-8-3"></a>

### Confusion<a id="sec-1-8-4" name="sec-1-8-4"></a>

### Blindness<a id="sec-1-8-5" name="sec-1-8-5"></a>

### Agony<a id="sec-1-8-6" name="sec-1-8-6"></a>

### Deafness<a id="sec-1-8-7" name="sec-1-8-7"></a>

# Developer's Guide<a id="sec-2" name="sec-2"></a>

## Source code structure<a id="sec-2-1" name="sec-2-1"></a>

### *actor.h*<a id="sec-2-1-1" name="sec-2-1-1"></a>

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
