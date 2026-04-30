# Unreal Engine network Passive Manager
Inhand network passive manager able to handle overtime effect, damage modification and editable. Handling logic, visuals and audio feedbacks.

## How it work ?
Before everything, all the system is based on the FPassiveDefinition. You'd rather create a Datatable from this.

To start, add or edit a value in the enum EPassiveClass, create a class inherating from UPassiveObject and then insert the case related to this EPassiveClasse in the UPassiveObject::MakePassive switch following the default case exemple, the only difference should be the template class which should be your new one. Everything is based on this.

In the constructor of the class set their StatusModifiers (Every passives of the same class are intend to work like the others).
The tricky part is the Duplication Type. All the logic is based on the LogicName. if you tried to add a new passive with the same LogicName than one already here, the type of "add" will depend:

  - None: cannot be applied, nothing happen
  - Refresh: Refresh the lifetime of the old one
  - Cumulative: Apply the new one unregarding to the old one

## How to edit the passive:
First, you can edit everything in the first created datatable.
To set what's happening:
  - During the tick, override UPassiveObject::OnTick function.
  - When applied, override UPassiveObject::OnApply function.
  - When removed, override UPassiveObject::OnRemove function.

🔴 **/!\ Warning:** ALWAYS CALL SUPER, This can break stacking logic

🔴 **/!\ Warning:** Think about authority

## How to use ?
This manager used to be on a pawn/character, but depending on how you tweak your passive you can put it on pretty much every actors, that's up to you.

To add a passive, gather the definition from the datatable or create one during runtime (which I don't recommend).
If Lifetime == -1: lifetime is infinite: use the FGuid to remove
