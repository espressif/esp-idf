#!/bin/bash
#
# 一键创建分支、推送并创建 PR 到 develop
#
# 用法: ./create-pr.sh
# 自动生成分支名: private/{用户名}/mergerequest.{YYYYMMDDHHmmss}
#
# 适配仓库: yungechen/esp-idf（默认分支 develop，开发分支 develop）
#

set -e

BASE_BRANCH="develop"

# ── 自动生成分支名 ──
GIT_USER=$(git config user.name | tr ' ' '_')
if [ -z "$GIT_USER" ]; then
    echo "错误: 未配置 git user.name，请先执行: git config user.name \"你的用户名\""
    exit 1
fi
TIMESTAMP=$(date +"%Y%m%d%H%M%S")
BRANCH_NAME="private/${GIT_USER}/mergerequest.${TIMESTAMP}"

# ── 检查依赖 ──
if ! command -v gh &> /dev/null; then
    echo "错误: 未安装 GitHub CLI (gh)，请先安装: https://cli.github.com/"
    exit 1
fi

# ── 检查未提交的更改 ──
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "错误: 存在未提交的更改，请先 commit 或 stash"
    git status --short
    exit 1
fi

# ── 检查分支是否已存在 ──
if git show-ref --verify --quiet "refs/heads/$BRANCH_NAME" 2>/dev/null; then
    echo "错误: 本地分支 '$BRANCH_NAME' 已存在"
    exit 1
fi

if git ls-remote --exit-code --heads origin "$BRANCH_NAME" &>/dev/null; then
    echo "错误: 远程分支 '$BRANCH_NAME' 已存在"
    exit 1
fi

# 标志：是否已通过智能迁移创建了分支
BRANCH_CREATED=0

# ── 智能迁移检查：如果本地 BASE 有未同步的提交，自动迁移到 PR 分支 ──
if [ "$(git branch --show-current)" = "$BASE_BRANCH" ]; then
    echo ">>> 检查本地 ${BASE_BRANCH} 分支状态..."
    git fetch origin "$BASE_BRANCH" --quiet

    LOCAL=$(git rev-parse "$BASE_BRANCH")
    REMOTE=$(git rev-parse "origin/$BASE_BRANCH")

    if [ "$LOCAL" != "$REMOTE" ]; then
        COMMIT_COUNT=$(git rev-list --count "origin/${BASE_BRANCH}..${BASE_BRANCH}")
        echo ">>> 检测到本地 ${BASE_BRANCH} 有 ${COMMIT_COUNT} 个未同步的提交"
        echo ">>> 将自动迁移到 PR 分支..."

        # 创建 patch 保存本地提交
        PATCH_FILE="/tmp/create-pr-patch-${TIMESTAMP}.patch"
        if ! git format-patch "origin/${BASE_BRANCH}..${BASE_BRANCH}" --stdout > "$PATCH_FILE"; then
            echo "错误: 创建 patch 失败"
            exit 1
        fi

        echo ">>> 重置本地 ${BASE_BRANCH} 到 origin/${BASE_BRANCH}..."
        git reset --hard "origin/${BASE_BRANCH}"

        echo ">>> 创建 PR 分支: $BRANCH_NAME"
        git checkout -b "$BRANCH_NAME"
        BRANCH_CREATED=1

        echo ">>> 应用 patch 到新分支..."
        if ! git am "$PATCH_FILE"; then
            echo "错误: patch 应用失败，patch 文件保留在: $PATCH_FILE"
            echo "请手动解决冲突"
            exit 1
        fi

        rm "$PATCH_FILE"
        echo ">>> 改动已成功迁移到分支: $BRANCH_NAME"
        echo ""
    fi
fi

# ── 步骤1: 创建新分支（如果尚未通过智能迁移创建） ──
if [ "$BRANCH_CREATED" -eq 0 ]; then
    echo ">>> 创建分支: $BRANCH_NAME"
    git checkout -b "$BRANCH_NAME"
fi

# ── 步骤2: 推送到远程 ──
echo ">>> 推送到远程..."
git push -u origin "$BRANCH_NAME"

# ── 步骤3: 生成 PR 标题和内容 ──
# 相对 origin/BASE 比较，避免本地 BASE 过旧导致范围不准
git fetch origin "$BASE_BRANCH" --quiet
COMPARE_BASE="origin/${BASE_BRANCH}"

COMMIT_COUNT=$(git rev-list --count "${COMPARE_BASE}..${BRANCH_NAME}" 2>/dev/null || echo "0")

# 取最新一条 commit 作为 PR 标题
PR_TITLE=$(git log "${COMPARE_BASE}..${BRANCH_NAME}" --pretty=format:"%s" -1 2>/dev/null || true)

# 如果标题为空，使用默认标题
if [ -z "$PR_TITLE" ]; then
    PR_TITLE="Merge ${BRANCH_NAME} into ${BASE_BRANCH}"
    echo "警告: 分支上没有提交，使用默认标题: $PR_TITLE"
fi

# 所有 commit 列表作为 PR 正文
COMMIT_LIST=$(git log "${COMPARE_BASE}..${BRANCH_NAME}" --pretty=format:"- %s (%h)" --reverse 2>/dev/null || echo "无提交记录")

echo ">>> 创建 Pull Request..."
echo "标题: $PR_TITLE"
gh pr create \
    --base "$BASE_BRANCH" \
    --head "$BRANCH_NAME" \
    --title "$PR_TITLE" \
    --body "$(cat <<EOF
## 变更内容

${COMMIT_LIST}

---
共 ${COMMIT_COUNT} 个提交
EOF
)"

echo ""
echo ">>> 完成！"
