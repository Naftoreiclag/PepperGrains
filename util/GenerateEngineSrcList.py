#   Copyright 2017 James Fong
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import os

from Common import indexFiles
sourceList, includeDirList, _ = \
    indexFiles('../src/pegr/', ['.cpp'], ['deprecated/'])

sourceListVector = '### SOURCE LIST ###'
includeDirListVector = '### INCLUDE DIR LIST ###'
outputFilename = '../cmake/EngineSrcList.cmake'
boilerplateFilename = 'EngineSrcListBoilerplate.cmake'

print('Sources: ' + str(len(sourceList)))
print('Directories: ' + str(len(includeDirList)))

with open(outputFilename, 'w') as outputFile:
    with open(boilerplateFilename, 'r') as boilerplateFile:
        for line in boilerplateFile:
            if line.startswith(sourceListVector):
                for sourceEntry in sourceList:
                    outputFile.write('\"' + sourceEntry + '\"\n')
            elif line.startswith(includeDirListVector):
                for includeDirEntry in includeDirList:
                    outputFile.write('\"' + includeDirEntry + '\"\n')
            else:
                outputFile.write(line)
