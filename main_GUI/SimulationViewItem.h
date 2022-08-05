#pragma once

#include "SimulationViewItem.g.h"

namespace winrt::main_GUI::implementation
{
    struct SimulationViewItem : SimulationViewItemT<SimulationViewItem>
    {
        SimulationViewItem() = default;


    };
}

namespace winrt::main_GUI::factory_implementation
{
    struct SimulationViewItem : SimulationViewItemT<SimulationViewItem, implementation::SimulationViewItem>
    {
    };
}
