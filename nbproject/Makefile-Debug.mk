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
CCC=DISTCC_FALLBACK=0 distcc
CXX=DISTCC_FALLBACK=0 distcc
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
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
	${OBJECTDIR}/src/AlertRessource.o \
	${OBJECTDIR}/src/InformationRessource.o \
	${OBJECTDIR}/src/PublicApiRessource.o \
	${OBJECTDIR}/src/AssetRessource.o \
	${OBJECTDIR}/src/Conf.o \
	${OBJECTDIR}/src/MediaRessource.o \
	${OBJECTDIR}/src/UserRessource.o \
	${OBJECTDIR}/src/ProbeRessource.o \
	${OBJECTDIR}/src/PluginRessource.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/var/lib/jenkins/jobs/ea-dbo/workspace/default/dist/Debug_SharedObject/GNU-Linux-x86 -L../dbo/dist/Debug_SharedObject/GNU-Linux-x86 -ldbo -lwt -lwtdbo -lwtdbopostgres -lwthttp -lboost_system -lboost_signals

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	distcc -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/api ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/itooki/ItookiSMSSender.o: src/itooki/ItookiSMSSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/itooki
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/itooki/ItookiSMSSender.o src/itooki/ItookiSMSSender.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/AlertRessource.o: src/AlertRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AlertRessource.o src/AlertRessource.cpp

${OBJECTDIR}/src/InformationRessource.o: src/InformationRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/InformationRessource.o src/InformationRessource.cpp

${OBJECTDIR}/src/PublicApiRessource.o: src/PublicApiRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PublicApiRessource.o src/PublicApiRessource.cpp

${OBJECTDIR}/src/AssetRessource.o: src/AssetRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AssetRessource.o src/AssetRessource.cpp

${OBJECTDIR}/src/Conf.o: src/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Conf.o src/Conf.cpp

${OBJECTDIR}/src/MediaRessource.o: src/MediaRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MediaRessource.o src/MediaRessource.cpp

${OBJECTDIR}/src/UserRessource.o: src/UserRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UserRessource.o src/UserRessource.cpp

${OBJECTDIR}/src/ProbeRessource.o: src/ProbeRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ProbeRessource.o src/ProbeRessource.cpp

${OBJECTDIR}/src/PluginRessource.o: src/PluginRessource.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Iinclude -I../dbo/include -I/var/lib/jenkins/jobs/ea-dbo/workspace/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PluginRessource.o src/PluginRessource.cpp

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
