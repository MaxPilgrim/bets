#! /bin/bash
# marathonbet.com
echo "*************************************Bets extraction*************************************"
mkdir -p data/
bash results/resshed.sh&
while true
do
	bash marathonbet.com/marathonbet.sh
	sleep 1800
done
echo "Bets extraction done."
