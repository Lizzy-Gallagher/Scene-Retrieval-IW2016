# SummerWork

This is a private repository for the Scene Understanding work from Summer 2016 I did with the Princeton Graphics Group.

###Directories:

- **bin** : scripts for running programs (see below for details)
- **data** : local version contains: Planner5D projects, generated statistics, ML training files
- **gaps** : holds Prof. Funhouser's C++ libraries and my apps (relview, scn2rel, scn2meas, p5danalyze)
- **prep-dist** : workspace for preposition distribution (many scenes)
- **prob-dist** : workspace for probability distribution (many scenes, heatmaps)
- **rel-proj** : workspace for relationship project (single or many scene, generates ML-ready files)

###Scripts:

#### runjob (Ionic)
- modify greatly for future use
- for running jobs on Ionic

#### rr (Run relview)
- use in a projects folder
- rr (builds dependencies and runs relview)
- rr clean / rr clean rel / rr clean meas

#### rl (Run Learning)
- use in a projects folder
- rl (builds dependencies and creates an .arff or .csv file)
- rl clean / rl clean rel/ rr clean meas

#### ml (Make Learning)
- run in folder with many .arff files and a header
- ml (concatenates the files into data.arff)

#### fl (Filter Learning)
- e.g. fl rug sofa
- creates another .arff file with only the 2 categories represented (for sanity checking)
