# Captures a screenshot of Wallmon executing on the Tromsø display wall
# Only 6 tiles of the display wall is included in the screen shot
curl "http://rocksvv.cs.uit.no:8008/wallshot?cropBox=(2048,768,5120,2304)" > ~/grabber/$(date +"%d-%m-%y_%T").png