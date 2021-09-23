# To do:

## In: bridge-firmwareOOP_incl_configv2

### Firmware code explanation

The UML diagram in this folder "splitcontroller_UML_diagram.png" shows the structure of the code.

- Initially an instance of "Nunchuckcontr" or "Modps4contr" is created based on which is defined. These are derived classes with "Controller" as base class for the common basis.
- To utilise custom mapping, in the beginning of the program two sets of struct are defined: one for the custom CSV mapping and one for the pin allocation. Of these structs, a custom std::map is created that can be updated with updated CSV instances in use.
- A sample mapping is already defined. This is to avoid null errors when the program is run without uploading a CSV before. Also, this way the std::map has to be initialized only once.
- the mapping in the std::map is defined as follows:
```
std::map<custCSVmapping, PinAllocation> mPinAlloc =
{
  {{green,left ,none,joyRXax2,repeat},{"joyRXax2", 2, DIGITALPIN}},
  ...
```
I chose the custCSVmapping as key and the PinAllocation as value because this works better with searching the std::map and updating later
- The "Controller" class has a method "operate()" in which buttons are read. The code of "operate()" itself hasn't changed since the original firmware yet, but reading the values has to be adapted to the std::map. For instance:
```
switch(LeftMode)
```
 Has to be changed to read the std::map key struct "mode" element. It won't change the logic, just a matter of renaming the variables.

 - That's about most of it. The original program worked but this rewrite makes adaptation and extensions easier with the use of OOP.
 Also the library NunchuckFunctions is rewritten and optimised by omitting superfluous code.

### To do: firmware

- Finetune existing code
  - Clean up SDreader class. This was dummy code so it has to be adapted for this projects' purpose
  - Adapt SDreader class to browse CSV files on SD.
  - Clean up CSV reader class.
  - Implement std::map mPinAlloc updating in CSV reader class method. Loop through map to look for element with something similar to following concise code:
  ```
    for (auto & element : pinalloc) {
        // if element.btn == csvElement.assgndto {
      // append mPinAlloc, csvElement, element}
        element.doSomething ();
      }
      ```

  - Update Controller class and derived classes to use the mPinAlloc mapping instead of original m_ps4pins in "operate()" and other methods


### CSV GUI program, code explanation

The CSV GUI program is a program that enables the user to create a mapping CSV file in a user friendly manner.

It is written in Python for convenience. It uses PyQt5 as GUI library.
Pandas is used for data. It has a library method to read and write DataFrames to CSV. Pandas will be used within the program to process the data.

### To do: Additional program with GUI

- Program with GUI with form fields
- generate correct dropdown items per row (eg: menu: red, green, blue or newly created. Button: list of defined buttons of the splitcontroller or ps4)
- Regex input validation
- I advise to make use of pyinstaller after completion of the script to make installation for the client easier.
