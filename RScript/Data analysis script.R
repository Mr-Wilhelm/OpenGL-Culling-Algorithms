#assign variables to the csv data being read
env1Data <- read.csv('/CSV/Envrion1Data')
env2Data <- read.csv('/CSV/Environ2Data')
env3Data <- read.csv('/CSV/Environ3Data')

#TODO: fix this to use our variables
# Compute the analysis of variance
res.aov <- aov(weight ~ group, data = my_data)
# Summary of the analysis
summary(res.aov