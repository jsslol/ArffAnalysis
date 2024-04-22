# ArffAnalysis
Reads training data from a weka arff file and stores them in data structures and then generates OneR outputs. OneR rule is in fact a set of 
rules that all based on value of a SINGLE attribute.  In addition the program also prints the error 
rate of the oneR rule (set) on the training set.  We assume that all attributes are nominal type and thus 
no discretization is required. 
# Performance Summary
weather.arff the error rate was 4/14 <br>
Restaurant.arff the error rate was 2/12 <br>
contactLense.arff the error rate was 7/24 <br>
