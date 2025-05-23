pipe = {
    new = function() end
}
process = {
    new = function(path, ...) end,
    bind_pipe = function (pipe, mode) end
    set_foreground = function(tf) end
    run = function () end
}
