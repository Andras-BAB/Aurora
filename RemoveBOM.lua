--
-- Remove UTF-8 BOM from all source files
--
function remove_bom_from_file(path)
    local file = io.open(path, "rb")
    if not file then return end

    local bytes = file:read(3)
    if bytes == "\239\187\191" then  -- EF BB BF
        print("Removing BOM from: " .. path)

        -- Read full content without BOM
        local content = file:read("*a")
        file:close()

        -- Rewrite file without BOM
        local out = io.open(path, "wb")
        out:write(content)
        out:close()
    else
        file:close()
    end
end

function remove_bom_recursive(dir)
    local p = io.popen('dir "' .. dir .. '" /b /s 2>nul')  -- Windows
    if not p then
        p = io.popen('find "' .. dir .. '" -type f 2>/dev/null') -- Linux/macOS
    end

    for file in p:lines() do
        if file:match("%.cpp$") or file:match("%.h$") then
            remove_bom_from_file(file)
        end
    end

    p:close()
end

-- Run automatically before project generation
premake.api.register {
    name = "remove_bom",
    scope = "workspace",
    kind = "boolean"
}

premake.override(premake.action, "call", function(base, action)
    local wks = premake.global.getWorkspace("Aurora")
    if wks and wks.remove_bom then
        print("Scanning for BOM...")
        remove_bom_recursive(".")
    end

    return base(action)
	
	
end)

--[[
premake.override(premake.action, "call", function(base, action)
    for _, wks in pairs(premake.global.getWorkspaces()) do
        if wks.remove_bom then
            print("Scanning for BOM in workspace: " .. wks.name)
            remove_bom_recursive(".")
        end
    end

    return base(action)
end)
]]
