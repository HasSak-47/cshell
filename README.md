# WHAT DOES NOT WORK AND WHY
trying to run an external command does not work
because the path of the cmd is not yet well done
and execv needs the path

needs that in *lua* determin the path first before trying to exec the cmd
