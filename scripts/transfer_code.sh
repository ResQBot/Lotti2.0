#! /bin/bash
ssh resqbots@lotti2-pc.local \
"cd Lotti2.0 && \
rm -r build/ && rm -r install/ && rm -r log/ && \
rm -r src/ && \
rm -r scripts && \
exit" 
scp -r ~/Lotti2.0/src/ resqbots@Lotti2-PC.local:/home/resqbots/Lotti2.0/
scp -r ~/Lotti2.0/scripts/ resqbots@Lotti2-PC.local:/home/resqbots/Lotti2.0/
