# Get-Rotated-Project
 Embedded System group project with Sam, Jess, Phoenix and Kyile

## Getting started, step 1.
If you are reading this README on your computer, not the internet, then it means you have already imported this repo. That means you only need to import a specific example project alongside it!
This repo was constructed by importing the example project from  the TIRTOS/reources example shown below:

![Importing include paths](./READMEResources/FreshProj.PNG "Get your project from here, and import it into the same folder as your github repo")

Yours will probably be located in the same place as mine:
C:\ti\tirtos_tivac_2_16_01_14\resources\ek_tm4C1294XLEvaluationKit\driverExamples\tiDriverExamples

### If you have not imported this git repo to your computer, then you can do that first, as shown, or you can create a project from that same example project, and then create the github repo alongside that

# TOPIC: Getting Started
## Getting started, step 2.

Make sure to use the github version of any files which are in conflict. This will mean deleting "Empty_min_ek_tm4C1294XL_ti".

I actually haven't gone through those steps I just described, but it seems reasonable. If it isn't working, just give me a message on Discord. Phoenix is better than me at this stuff too, so I hope you aren't Phoenix AND having trouble, because then we are really stuck.

### Your project will not be compiling and running yet; that is OK.

## Getting started, step 3.

If you are happy to create your own work environment or have one set up already, that is excellent. But these settings will pre-allocate include paths of everything which is part of this git repo. What it DOESN'T do is correctly identify any resources which are part of Texas Instruments, TIVA, RTOS etc. 

So, you can import these settings as described below, but then if the project doesn't build perfectly, that means you will simply have to change where THOSE resources are, such as on your D:/ drive

![Importing include paths](./READMEResources/Importing-Include-Paths.PNG "How to import settings which MOSTLY will work, even on D-drive")


![Find A premade file of includes](./READMEResources/Location of Include Paths XML.PNG "Here is where the xml is on this git repo")

## How are we looking?

![Environment as it started](./READMEResources/FirstEnvironment.PNG "After you are ready, and after you have debugged once, your environment might look like mine, which is shown here as it was when I did the first commit.")


# TOPIC: The .gitignore.
I am not sure if we can safely put any of our existing files, like main.c, into a folder, but lets put every other custom resource into a folder in the future, so that we can avoid getting the top-level anymore messy. If anyone does take on the task of cleaning all that up, you will have to re-create the xml file in step 1, above.

For now, I have created the .gitignore to ignore EVERYTHING by default, and you have to explicitly allow exceptional files (I know all your files will be exceptional, because we are a smart crew).


Here is an example, with an exclusion for main, and a folder. Apply your exceptions just like that
![Importing include paths](./READMEResources/Altering-the-gitignore.PNG "Basic idea of altering which files are shared, to avoid screwing up other peoples environments.")

# TOPIC: LiveShare
https://code.visualstudio.com/learn/collaboration/live-share

Liveshare has to be initiated like a livestream, 
so there is a host, who starts it, but you can actually mess around with the code on my screen. You don't even need to have the git repo on your computer for it to work (anymore than you would need Fortnite to get that livestreamed toy our phone).

You do need a github account, though. So go get that real quick and come back.

### You do NOT need to do anything special in VSCode to open and edit a CCS file. You open the .c, edit it, and it is instantly updated in C.C.S., so you can isntantly compile and run/debug

## Install it here in VSCode:

![Downloading Liveshare](./READMEResources/LiveShare.PNG " DO ITTTTT")


