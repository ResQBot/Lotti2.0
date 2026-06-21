#! /bin/bash
ssh resqbots@lotti2-pc.local
source ~/.bashrc && \
cd Lotti2.0 && \
remro && \
rm -r src/ && \
exit 
scp -r ~/Lotti2.0/src/ resqbots@Lotti2-PC.local:/home/resqbots/Lotti2.0/