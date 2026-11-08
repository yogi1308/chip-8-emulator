vim.g.c_syntax_for_h = true
local function run_in_bottom_terminal(command)
	local terminal_win
	local terminal_row = -1

	for _, win in ipairs(vim.api.nvim_list_wins()) do
		local config = vim.api.nvim_win_get_config(win)
		local buf = vim.api.nvim_win_get_buf(win)
		local row = vim.api.nvim_win_get_position(win)[1]
		local height = vim.api.nvim_win_get_height(win)

		if config.relative == "" and vim.bo[buf].buftype == "terminal" and row + height >= vim.o.lines - 3 then
			if row > terminal_row then
				terminal_win = win
				terminal_row = row
			end
		end
	end

	if terminal_win then
		vim.api.nvim_set_current_win(terminal_win)
		local buf = vim.api.nvim_win_get_buf(terminal_win)
		local job_id = vim.b[buf].terminal_job_id
		if job_id then
			local sent = pcall(vim.fn.chansend, job_id, command .. "\n")
			if sent then
				return
			end
		end

		local new_job_id = vim.fn.termopen(vim.o.shell)
		vim.fn.chansend(new_job_id, command .. "\n")
		return
	end

	vim.cmd("botright split")
	vim.cmd("resize 25")
	vim.cmd("terminal")
	vim.fn.chansend(vim.b.terminal_job_id, command .. "\n")
end

vim.keymap.set("n", "<leader>cb", function()
	run_in_bottom_terminal("make")
end, { desc = "Build with Makefile" })

vim.keymap.set("n", "<leader>cr", function()
	run_in_bottom_terminal("make run")
end, { desc = "Run with Makefile" })

vim.keymap.set("n", "<leader>cx", function()
	local function tail(res)
		local lines = {}
		for line in ((res.stdout .. res.stderr):gsub("\r", "")):gmatch("[^\n]+") do
			lines[#lines + 1] = line
		end
		return table.concat(lines, "\n", math.max(1, #lines - 7))
	end

	local function stage(cmd, running, ok, next_fn)
		vim.notify(running)
		vim.system(cmd, { text = true }, vim.schedule_wrap(function(res)
			if res.code == 0 then
				vim.notify(type(ok) == "function" and ok(res) or ok)
				next_fn()
			else
				local out = tail(res)
				vim.notify(running .. " failed:\n" .. (out ~= "" and out or "no output"), vim.log.levels.ERROR)
			end
		end))
	end

	stage({ "make", "clean" }, "Running make clean...", "Cleaned successfully", function()
		stage({ "make" }, "Building...", "Build successful", function()
			stage({ "make", "run" }, "Running...", function(res)
				return "Run successful\n" .. tail(res)
			end, function() end)
		end)
	end)
end, { desc = "Clean, build and run (background)" })

vim.keymap.set("n", "<leader>ct", function()
	run_in_bottom_terminal("make clean && make run")
end, { desc = "Clean, build and run with Makefile" })

vim.keymap.set("n", "<leader>cd", function()
	vim.notify("make debug: building...")
	vim.system({ "make", "debug" }, { text = true, cwd = vim.fn.getcwd() }, function(res)
		vim.schedule(function()
			if res.code == 0 then
				vim.notify("make debug: success")
			else
				local out = (res.stderr ~= "" and res.stderr) or res.stdout
				vim.notify("make debug failed:\n" .. out, vim.log.levels.ERROR)
			end
		end)
	end)
end, { desc = "Build debug with Makefile (background)" })
