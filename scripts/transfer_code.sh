#! /bin/bash

PS3="Select device: "

items=("Lotti-PC" "Operator-Laptop")

select item in "${items[@]}" Quit
do
    case $REPLY in
        1) _device_="lotti2-pc";break;;
        2) _device_="resqbots-PC";break;;
        $((${#items[@]}+1))) echo "We're done!"; break;;
        *) echo "Ooops - unknown choice $REPLY";;
    esac
done

ssh resqbots@$_device_.local \
"cd Lotti2.0; \
rm -r build/ install/ log/ src/ scripts; \
exit" 
scp -r ~/Lotti2.0/src/ resqbots@Lotti2-PC.local:/home/resqbots/Lotti2.0/ &&
scp -r ~/Lotti2.0/scripts/ resqbots@Lotti2-PC.local:/home/resqbots/Lotti2.0/ &&
ssh resqbots@$_device_.local \
"source ~/.bashrc; \
cd Lotti2.0; \
cbs; \
exit;"