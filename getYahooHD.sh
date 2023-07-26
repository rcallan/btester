#!/bin/bash -eu                                                                                                       
                                                                                                                      
# Client script to pull Yahoo Finance historical data, off of its new cookie                                          
# authenticated site. Start/End Date args can be any GNU readable dates.                                              
# Script requires: GNU date, curl and bash shell                                                                      
                                                                                                                      
# symbol=$1                                                                                                             
startDate=$2                                                                                                          
endDate=$3                                                                                                            
                                                                                                                      
startEpoch=$(date -d "$startDate" '+%s')                                                                              
endEpoch=$(date -d "$endDate" '+%s')                                                                                  
                                                                                                                      
cookieJar=$(mktemp)                                                                                                   
function cleanup() {                                                                                                  
    rm $cookieJar                                                                                                     
}                                                                                                                     
trap cleanup EXIT                                                                                                     
                                                                                                                      
function parseCrumb() {                                                                                               
    sed 's+}+\n+g' | grep CrumbStore | cut -d ":" -f 3 | sed 's+"++g'                                                                                                
}                                                                                                                                                                                                                                         
                                                                                                                      
baseUrl="https://query1.finance.yahoo.com/v7/finance/download/"                                                       

# the command we pass to wget
# for i in $(sed -e "s:,: :" -e "/Symbol*/ d" -e "/^$/ d" $1 | awk '{print $1}'); do
while IFS="" read -r i || [ -n "$i" ]
do

echo "Downloading the data for symbol" $i

function extractCrumb() {                                                                                             
    crumbUrl="https://ca.finance.yahoo.com/quote/$i/history?p=$i"
    curl -s --cookie-jar $cookieJar $crumbUrl | parseCrumb                                                                                                 
}

crumb=$(extractCrumb)                                                                                                 
crumbArg="&crumb=$crumb"
# args="$symbol?period1=$startEpoch&period2=$endEpoch&interval=1d&events=history"                                       
args="$i?period1=$startEpoch&period2=$endEpoch&interval=1d&events=history"
sheetUrl="$baseUrl$args$crumbArg"

output="sample_inputs/${i}.csv"
                                                                                                                      
curl -s --cookie $cookieJar "$sheetUrl" -o $output

done < $1