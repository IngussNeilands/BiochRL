#include "stdafx.h"
#include "elements_t.h"
#include <unordered_map>

TCODColor get_element_color(elements_t element)
{
    std::unordered_map<elements_t, TCODColor> spell_color = std::unordered_map<elements_t, TCODColor>();

    spell_color.insert(std::make_pair<elements_t, TCODColor>(FireElement, TCODColor::red));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(WaterElement, TCODColor::blue));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(LifeElement, TCODColor::lightGreen));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(DeathElement, TCODColor::darkerGrey));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(SpectreElement, TCODColor::darkPurple));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(CrystalElement, TCODColor::magenta));


    return spell_color.at(element);
}
