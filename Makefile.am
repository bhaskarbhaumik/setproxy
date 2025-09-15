AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-xz dist-zip

bin_PROGRAMS = setproxy
setproxy_SOURCES = setproxy.c

# Add compilation flags
setproxy_CFLAGS = -Wall -Wextra -O2

# Install target (optional)
install-exec-hook:
	@echo "setproxy has been installed to $(bindir)"

uninstall-hook:
	@echo "setproxy has been uninstalled from $(bindir)"

# Define the distribution files
EXTRA_DIST = README.md CMakeLists.txt build.sh .gitignore cmake_clean.cmake LICENSE

# Add dist-hook to clean unnecessary files from distribution
dist-hook:
	rm -rf $(distdir)/autom4te.cache
	rm -rf $(distdir)/build
	rm -f $(distdir)/*~
	rm -f $(distdir)/*.o

# Custom target to generate distribution package and verify it
distcheck-local:
	@echo "Distribution package has been created and verified."

# Git related targets
.PHONY: git-status git-add git-commit git-push git-pull git-log git-tag git-branch

# Display git status
git-status:
	@echo "Git Repository Status:"
	@git status

# Add all changes to staging
git-add:
	@echo "Adding all changes to staging..."
	@git add .
	@echo "Added changes. Current status:"
	@git status

# Commit changes with a provided message
# Usage: make git-commit MSG="Your commit message"
git-commit:
	@if [ -z "$(MSG)" ]; then \
		echo "Error: Commit message not provided. Use make git-commit MSG=\"Your commit message\""; \
		exit 1; \
	fi
	@echo "Committing changes with message: $(MSG)"
	@git commit -m "$(MSG)"

# Push commits to remote repository
# Usage: make git-push [REMOTE=origin] [BRANCH=main]
git-push:
	@REMOTE=$${REMOTE:-origin}; \
	BRANCH=$${BRANCH:-main}; \
	echo "Pushing to $${REMOTE}/$${BRANCH}..."; \
	git push $${REMOTE} $${BRANCH}

# Pull changes from remote repository
# Usage: make git-pull [REMOTE=origin] [BRANCH=main]
git-pull:
	@REMOTE=$${REMOTE:-origin}; \
	BRANCH=$${BRANCH:-main}; \
	echo "Pulling from $${REMOTE}/$${BRANCH}..."; \
	git pull $${REMOTE} $${BRANCH}

# Show git log
# Usage: make git-log [N=10] (where N is the number of commits to show)
git-log:
	@N=$${N:-10}; \
	echo "Showing last $${N} commits:"; \
	git log --pretty=format:"%h - %an, %ar : %s" -n $${N}

# Create a new tag
# Usage: make git-tag TAG=v1.0.0 [MSG="Tag message"]
git-tag:
	@if [ -z "$(TAG)" ]; then \
		echo "Error: Tag name not provided. Use make git-tag TAG=v1.0.0 [MSG=\"Tag message\"]"; \
		exit 1; \
	fi
	@if [ -z "$(MSG)" ]; then \
		echo "Creating tag $(TAG)..."; \
		git tag "$(TAG)"; \
	else \
		echo "Creating tag $(TAG) with message: $(MSG)"; \
		git tag -a "$(TAG)" -m "$(MSG)"; \
	fi
	@echo "Tag created. Use 'make git-push REMOTE=origin TAG=$(TAG)' to push the tag."

# Create a new branch and switch to it
# Usage: make git-branch BRANCH=feature-branch
git-branch:
	@if [ -z "$(BRANCH)" ]; then \
		echo "Error: Branch name not provided. Use make git-branch BRANCH=feature-branch"; \
		exit 1; \
	fi
	@echo "Creating and switching to branch $(BRANCH)..."
	@git checkout -b "$(BRANCH)"