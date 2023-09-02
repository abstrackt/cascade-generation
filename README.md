# cascade-generation

This is a supplimentary tool designed to generate filtering cascades for the MetaJSONParser project.

To run the executable from the main project folder:

```
cd src
make
./cascades <JSON PATH> <FILTER STATEMENT> <NUMBER OF SAMPLES>
```

This produces a string as output which contains ASCII codes of characters in the 2D cascade array. 

You can paste this output directly as the initialization value for the constexpr in the cascade configuration file of meta-json-parser.

To clean build files

```
cd src
make clean
```


