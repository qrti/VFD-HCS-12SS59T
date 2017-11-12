# Arduino Driver for Samsung HCS-12SS59T VFDisplay
quick and simple schematics, driver and example

## 

![HCS-12SS59T in action](images/display.jpg)

## Code
-> source directory

## Schematics
### Display
![display schematics](images/s_display.png)

- for simple DC filament supply  
  connect Vcc to VF+  
  mix normal and schottky diodes on VF- for desired voltage  
  keeping VF low extends tungsten filament lifetime  
  voltages starting with 2.7 V up to 3.6 V are working  
  do not exceed 125 mA!
- -> [AC Filament Supply](#ac-filament-supply) 

### Anode and Grid Supply
![anode + grid supply schematics](images/s_anode_grid_supply.png)

- activate Vdisp  
  in VFD.h set `SUPPLYMODE 0`
- the simple _VDON with 1N4148 does not switch off Vdisp entirely,  
  maybe it can be omitted because the 470 ohm resistor on Vdisp  
  restricts current for VFD grid and anode

### AC Filament Supply
![ac filament supply schematics](images/s_ac_filament_supply.png)

- activate Vdisp  
  in VFD.h set `SUPPLYMODE 0`
- extended filament endurance  
  (-> google filament notching)
- better brightness distribution  
  (theoretical, can't see any difference to DC supply on this display)
- coil-less

### Combined Clocked Supply
![clocked supply schematics](images/s_clocked_supply.png)

- activate SUCLK_A/B  
  in VFD.h set `SUPPLYMODE 1`

## History
__V0.5__  
initial version  
__V0.6__  
AC filament supply  
__V0.7__  
AC filament supply _VDON  
__V0.8__  
AC filament supply polarity correction  
__V0.9__  
combined clocked supply

## Links
http://www.lapis-semi.com/en/data/datasheet-file_db/display/FEDL9289-01.pdf  
https://www.pollin.de/productdownloads/D121466D.PDF  
https://www.noritake-elec.com/technology/general-technical-information/vfd-operation  
http://www.kerrywong.com/2013/06/05/vfd-filament-driver-using-555/  
https://threeneurons.wordpress.com/vfd-stuff/  
