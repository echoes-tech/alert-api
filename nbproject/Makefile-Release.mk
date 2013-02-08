#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/itooki/ItookiSMSSender.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/AssetRessource.o \
	${OBJECTDIR}/src/Conf.o \
	${OBJECTDIR}/src/ProbeRessource.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++0x
CXXFLAGS=-std=c++0x

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/var/lib/jenkins/jobs/ea-dbo-develop/workspace/default/dist/Release_SharedObject/GNU-Linux-x86 -L../dbo/dist/Release_SharedObject/GNU-Linux-x86 -lwt -lwtdbo -lwtdbopostgres -lwtfcgi -ldbo -lboost_filesystem -lboost_signals -lboost_system

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api -s ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/itooki/ItookiSMSSender.o: src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-develop/workspace/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-develop/workspace/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/AssetRessource.o: src/AssetRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-develop/workspace/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AssetRessource.o src/AssetRessource.cpp

${OBJECTDIR}/src/Conf.o: src/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-develop/workspace/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Conf.o src/Conf.cpp

${OBJECTDIR}/src/ProbeRessource.o: src/ProbeRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo-develop/workspace/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ProbeRessource.o src/ProbeRessource.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
