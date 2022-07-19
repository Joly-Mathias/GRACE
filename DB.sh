#! /bin/bash

tableau=(google amazon facebook apple microsoft wikipedia youtube instagram twitter ecosia)
echo -n "" > DB.txt
for x in ${tableau[@]}
do
    echo $x.fr >> DB.txt
    echo $x.eu >> DB.txt
    echo $x.com >> DB.txt
    echo $x.net >> DB.txt
    echo $x.org >> DB.txt
    echo $x.php >> DB.txt
    echo $x.jpg >> DB.txt
    echo $x.png >> DB.txt
    echo $x.it >> DB.txt
    echo $x.en >> DB.txt
done