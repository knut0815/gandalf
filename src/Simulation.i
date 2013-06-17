/* File : Simulation.i */
%module SphSim
%include "std_string.i"
%include "std_map.i"
%include "std_list.i"
%include exception.i

%{
#define SWIG_FILE_WITH_INIT
#include "Exception.h"
#include "Simulation.h"
#include "Parameters.h"
#include "SimUnits.h"
#include "Sph.h"
#include "SphSnapshot.h"
#include <signal.h>
#include <string>
#include "Precision.h"
#include "Render.h"
#include "SphKernel.h"
#include "UnitInfo.h"

void catch_alarm (int SIG) {
signal(SIGINT, catch_alarm);
throw StopError("CTRL-C received");
}
%}

%exception SimulationBase::InteractiveRun {
	signal(SIGINT, catch_alarm);
	PyThreadState *_save;
    _save = PyEval_SaveThread();
    try{
        $action
        PyEval_RestoreThread(_save);
    }
    catch (StopError e){
    	PyEval_RestoreThread(_save);
    	PyErr_SetString(PyExc_KeyboardInterrupt,e.msg.c_str());
    	return NULL;
    }
    catch (SerenError e){
    	PyEval_RestoreThread(_save);
    	PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
    }
}

%exception SimulationBase::GetParam {
	try{
		$action
	}
	catch (SerenError e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception SimulationBase::SetParam {
	try{
		$action
	}
	catch (SerenError e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception SimulationBase::PreSetupForPython {
	try{
		$action
	}
	catch (SerenError &e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception SimulationBase::SetupSimulation {
	try{
		$action
	}
	catch (SerenError &e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception SimulationBase::ImportArray {
	try {
		$action
	}
	catch (SerenError &e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception Parameters::ReadParamsFile {
	try{
		$action
	}
	catch (SerenError &e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;		
	}
}

%exception SphSnapshotBase::ExtractArray {
	try{
		$action
	}
	catch (SerenError &e) {
		PyErr_SetString(PyExc_Exception,e.msg.c_str());
		return NULL;
	}
}

%exception RenderBase::CreateColumnRenderingGrid {
	signal(SIGINT, catch_alarm);
	PyThreadState *_save;
    _save = PyEval_SaveThread();
    try{
        $action
        PyEval_RestoreThread(_save);
    }
    catch (StopError e){
    	PyEval_RestoreThread(_save);
    	PyErr_SetString(PyExc_KeyboardInterrupt,e.msg.c_str());
    	return NULL;
    }
}

%exception RenderBase::CreateSliceRenderingGrid {
	signal(SIGINT, catch_alarm);
	PyThreadState *_save;
    _save = PyEval_SaveThread();
    try{
        $action
        PyEval_RestoreThread(_save);        
    }
    catch (StopError e){
        PyEval_RestoreThread(_save);
    	PyErr_SetString(PyExc_KeyboardInterrupt,e.msg.c_str());
    	return NULL;
    }
}
    
%include "numpy.i"
%init %{
import_array();
ExceptionHandler::makeExceptionHandler(python);
%}
%numpy_typemaps(float, NPY_FLOAT, int)
 /* %include <boost_any.i> */
 
 namespace std {
    %template(map_string_int) map<string, int>;
    %template(map_string_string) map<string, string>;
    %template(map_string_float) map<string, float>;
    %template(list_snap_base_pointer) list<SphSnapshotBase* >;
    /*%template(map_string_any) map<string, boost::any>;*/
}

 /* Applies Numpy black magic */
 %apply (float** ARGOUTVIEW_ARRAY1, int *DIM1) {(float** out_array, int* size_array)}
 %apply (float* INPLACE_ARRAY1, int DIM1) {(float* values, int Ngrid)}
 %apply (double* IN_ARRAY1, int DIM1) {(double* input, int size)}
 
 %apply float& OUTPUT { float& scaling_factor };
 
%include "Precision.h"
%include "Simulation.h"
%include "Parameters.h"
%include "SimUnits.h"
%include "Sph.h"
%include "SphSnapshot.h"
%include "Render.h"
%include "SphKernel.h"
%include "UnitInfo.h"