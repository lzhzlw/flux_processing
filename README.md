The code repository for flux tower data processing. For more information on flux data, take a detour to the [Wiki](https://github.com/lzhzlw/flux_processing/wiki)!

## File description

### split_TOA5.c
Split the raw Campbell TOA5 flux data into 30-min intervals with high speed and batch processing capability. The speed is 20 times that of LoggerNet.

### flux2biomet.ipynb
Extract biomet variables from raw Campbell TOA5 data and convert them into a format compatible with Li-Cor EddyPro. The header names follow Li-Cor's instructions: [link](https://www.licor.com/support/EddyPro/topics/biomet-data-format.html). Keep an eye on the variable units!
