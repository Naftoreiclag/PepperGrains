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

import xml.etree.ElementTree as ET
import os

from Common import indexFiles
sourceList, _, __ = \
    indexFiles('../src/pegr/', ['.cpp'], ['deprecated/'])
includeList, _, __ = \
    indexFiles('../src/pegr/', ['.hpp'], ['deprecated/'])

projFilename = '../ide/VS/VS.vcxproj'
filtFilename = '../ide/VS/VS.vcxproj.filters'
sourceRootRelativePath = '../../src/pegr/'

def convertToProjectPath(path):
    return os.path.join(sourceRootRelativePath, path).replace('/', '\\')

sourceList = \
    [(convertToProjectPath(source), source) for source in sourceList]
includeList = \
    [(convertToProjectPath(include), include) for include in includeList]

projEtree = ET.parse(projFilename)
projRoot = projEtree.getroot()

import re
schemaPattern = re.compile('\{(\S*)\}')
schemaMatch = schemaPattern.match(projRoot.tag)
if schemaMatch:
    schemaPrefix = schemaPattern.match(projRoot.tag).group(1)
    ET.register_namespace('', schemaPrefix)
    projEtree = ET.parse(projFilename)
    projRoot = projEtree.getroot()

filtEtree = ET.parse(filtFilename)
filtRoot = filtEtree.getroot()

def qualify(tag):
    return '{' + schemaPrefix + '}' + tag

def findItemGroupWithNamedChildren(root, childName):
    for itemGroupElem in root.findall(qualify('ItemGroup')):
        childElems = itemGroupElem.findall(childName)
        if childElems:
            return itemGroupElem, childElems
    return ET.SubElement(root, qualify('ItemGroup')), []

projCompileItemGroupElem, projCompileElems = \
    findItemGroupWithNamedChildren(projRoot, qualify('ClCompile'))
projIncludeItemGroupElem, projIncludeElems = \
    findItemGroupWithNamedChildren(projRoot, qualify('ClInclude'))
filtFilterItemGroupElem, filtFilterElems = \
    findItemGroupWithNamedChildren(filtRoot, qualify('Filter'))
filtCompileItemGroupElem, filtCompileElems = \
    findItemGroupWithNamedChildren(filtRoot, qualify('ClCompile'))
filtIncludeItemGroupElem, filtIncludeElems = \
    findItemGroupWithNamedChildren(filtRoot, qualify('ClInclude'))

vsProjCompileList = [x.get('Include') for x in projCompileElems]
vsProjIncludeList = [x.get('Include') for x in projIncludeElems]
vsFiltCompileList = [x.get('Include') for x in filtCompileElems]
vsFiltIncludeList = [x.get('Include') for x in filtIncludeElems]
vsFilterList = set(x.get('Include') for x in filtFilterElems)

missingFilters = set()
for sourcePath, sourceName in sourceList:
    filterName = 'pegr\\' + os.path.split(sourceName)[0].replace('/', '\\')
    
    if sourcePath not in vsProjCompileList:
        # Add a new entry into the project file
        newElem = ET.SubElement(projCompileItemGroupElem, qualify('ClCompile'))
        newElem.set('Include', sourcePath)
        
    if sourcePath not in vsFiltCompileList:
        # Add a new entry into the filters file
        newElem = ET.SubElement(filtCompileItemGroupElem, qualify('ClCompile'))
        newElem.set('Include', sourcePath)
        filterElem = ET.SubElement(newElem, qualify('Filter'))
    else:
        for elem in filtCompileItemGroupElem:
            if elem.get('Include') == sourcePath:
                filterElem = ET.SubElement(elem, qualify('Filter'))
                break
    
    filterElem.text = filterName
        
    missingFilters.add(filterName)
for includePath, includeName in includeList:
    filterName = 'pegr\\' + os.path.split(includeName)[0].replace('/', '\\')
    if includePath not in vsProjIncludeList:
        # Add a new entry into the project file
        newElem = ET.SubElement(projIncludeItemGroupElem, qualify('ClInclude'))
        newElem.set('Include', includePath)
        
    if includePath not in vsFiltCompileList:
        # Add a new entry into the filters file
        newElem = ET.SubElement(filtIncludeItemGroupElem, qualify('ClInclude'))
        newElem.set('Include', includePath)
        filterElem = ET.SubElement(newElem, 'Filter')
    else:
        for elem in filtIncludeItemGroupElem:
            if elem.get('Include') == includePath:
                filterElem = ET.SubElement(elem, qualify('Filter'))
                break
    
    filterElem.text = filterName
        
    missingFilters.add(filterName)

import uuid

missingImplicitFilters = set()
for filterName in missingFilters:
    while True:
        head, tail = os.path.split(filterName)
        if head:
            missingImplicitFilters.add(head)
            filterName = head
        else:
            break
missingFilters |= missingImplicitFilters

for filterName in missingFilters:
    if not filterName or filterName in vsFilterList:
        continue
    filterElem = ET.SubElement(filtFilterItemGroupElem, qualify('Filter'))
    filterElem.set('Include', filterName)
    
    uniqueElem = ET.SubElement(filterElem, qualify('UniqueIdentifier'))
    uniqueElem.text = '{' + str(uuid.uuid4()) + '}'

projEtree.write(projFilename)
filtEtree.write(filtFilename)
