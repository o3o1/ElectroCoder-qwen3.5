@echo off
cd /d "%~dp0"
powershell -Command "& {python task_vf_control_python.py}"