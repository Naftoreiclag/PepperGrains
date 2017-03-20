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

searchPath = '../src/pegr/'
includePrefix = 'pegr/'

from Common import indexFiles
headerFnames, _, filenameToPath = \
    indexFiles(searchPath, ['.hpp', '.cpp'], [])

lowercaseFnameToPath = {}
for filename, path in filenameToPath.items():
    lowercaseFnameToPath[filename.lower()] = path
filenameToPath = lowercaseFnameToPath
    
import re

includePattern = re.compile('#include\s+["<](\S*)[">]')

for headerFname in headerFnames:
    with open(searchPath + headerFname, 'r') as headerFile:
        headerLines = headerFile.readlines()
    
    changesMade = False
    for lineIdx in range(len(headerLines)):
        headerLine = headerLines[lineIdx]
        replaceLine = headerLine
        match = includePattern.match(headerLine)
        if match:
            originalInclude = match.group(1)
            _, includeFname = os.path.split(originalInclude)
            includeFname = includeFname.lower()
            
            if includeFname in filenameToPath:
                replaceInclude = includePrefix + filenameToPath[includeFname]
                replaceLine = '#include "' + replaceInclude + '"\n'
            else:
                replaceLine = '#include <' + originalInclude + '>\n'
        
        if replaceLine != headerLine:
            if not changesMade:
                print(headerFname)
                changesMade = True
            headerLines[lineIdx] = replaceLine
            print('\t' + headerLine[:-1] + ' --> ' + replaceLine[:-1])
    if changesMade:
        with open(searchPath + headerFname, 'w') as headerFile:
            headerFile.writelines(headerLines)
