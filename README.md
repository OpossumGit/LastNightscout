# LastNightscout
Pebble watchface for Nightscout

LastNightscout is Pebble and Pebble Time watchface designed to display latest collected data from your [Nightscout](https://github.com/nightscout/cgm-remote-monitor) diabetes management instance. This watchface is created to support use case of Abbott Freestyle Libre flash glucose monitor connected to Nightscout ecosystem. Nightscout was initially designed to support CGM systems and as such expects to gather readings continuously. For that reason it will warn you when it has not received data recently. For same reason, other Pebble watchfaces I tried were warning me when there were no recent results and some additionally refused to display "outdated" data. For that reason I created LastNightscout.

It also displays battery status and warns of disconnections.
