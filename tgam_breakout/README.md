TGAM Breakout
=============

For Neurosky ThinkGear ASIC Module. breaks out the 2.0mm pitch pins from the
TGAM board to coax and 2.54mm pitch connectors. Has electrode pads too.

Seeed did produce a batch of these for me, they did ask me to doublecheck since this does not pass the DRU but the result was fine. Remember to order in ENIG finish.

## BOM

  - 1 TGAM: http://www.seeedstudio.com/depot/thinkgear-am-brainwave-sensor-p-1441.html?cPath=25_29
  - 1 6 contact pin header 2.0mm: http://www.digikey.fi/product-detail/en/M22-2020305/952-2133-ND/3728097
  - 1 5 contact pin header 2.0mm: http://www.digikey.fi/product-detail/en/M22-2010505/952-2276-ND/3728240
  - 3 UMC cables: http://www.digikey.fi/product-detail/en/2015357-4/A36229-ND/1249193
  - 6 UMC connectors: http://www.digikey.fi/product-detail/en/0734120114/WM3894CT-ND/2421931

Instead of the expensive coax cable and connectors you can use shielded
audio cable and solder that directly on the board, the signal wire goes
on the coax middle (the trace is easy to identify) and shield goes to
either of the two pads of the coax connector shields. 
