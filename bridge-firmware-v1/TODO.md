To do:
=============

In: bridge-firmwareOOP_incl_configv2
-------------
- std::map<custCSVmapping, PinAllocation> mPinAlloc   has to be assigned properly. It has to be initialized with a standard mapping, customized when needed.
- Finetune existing code
  - Clean up SDreader class.
  - Adapt SDreader class to browse CSV files on SD.
  - Clean up CSV reader class.
  - Safely update mPinAlloc from CSVreader method
  - Update Controller class and derived classes to use the mPinAlloc mapping instead of original m_ps4pins
-

In: Additional program with GUI
-------------
- Program with GUI with form fields
- Regex input validation
- To be discussed whether to be implemented:
  - read CSV file ( --> store data in struct format )
- Read input (use struct)
- Input fields --> write in CSV format to file
